#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <pthread.h>

#include "teller.h"
#include "account.h"
#include "error.h"
#include "debug.h"
#include "branch.h"
#include "account.h"

#define post pthread_mutex_unlock
#define wait pthread_mutex_lock
#define ERROR_SAME_PERSON 9999

/*
 * deposit money into an account
 * 
 * Lock and post before return.
 */
int
Teller_DoDeposit(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoDeposit(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);

  if (account == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  BranchID id = AccountNum_GetBranchID(account->accountNumber);
  mut(wait,2,&bank->branches[id].b_lock,&account->b_lock);
  Account_Adjust(bank, account, amount, 1);
  mut(post,2,&account->b_lock,&bank->branches[id].b_lock);
  return ERROR_SUCCESS;
}

/*
 * withdraw money from an account
 * 
 * Same logic, but we need to check if account has enough money.
 */
int
Teller_DoWithdraw(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoWithdraw(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);

  if (account == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  BranchID id = AccountNum_GetBranchID(account->accountNumber);
  mut(wait,2,&bank->branches[id].b_lock,&account->b_lock);

  if (amount > Account_Balance(account)) {
    mut(post,2,&account->b_lock,&bank->branches[id].b_lock);
    return ERROR_INSUFFICIENT_FUNDS;
  }
  Account_Adjust(bank, account, -amount, 1);
  mut(post,2,&account->b_lock,&bank->branches[id].b_lock);
  return ERROR_SUCCESS;
}

/*
 * do a tranfer from one account to another account
 */
int
Teller_DoTransfer(Bank *bank, AccountNumber srcAccountNum,
                  AccountNumber dstAccountNum,
                  AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoTransfer(src 0x%"PRIx64", dst 0x%"PRIx64
                ", amount %"PRId64")\n",
                srcAccountNum, dstAccountNum, amount));

  Account *srcAccount = Account_LookupByNumber(bank, srcAccountNum);
  if (srcAccount == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  Account *dstAccount = Account_LookupByNumber(bank, dstAccountNum);
  if (dstAccount == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  //We can check this before actual transfer
  pthread_mutex_lock(&srcAccount->b_lock);
  if (amount > Account_Balance(srcAccount)) {
    pthread_mutex_unlock(&srcAccount->b_lock);
    return ERROR_INSUFFICIENT_FUNDS;
  }
  pthread_mutex_unlock(&srcAccount->b_lock);

  //We need to make sure src and dst isnt the same account
  if(srcAccount->accountNumber == dstAccount->accountNumber) return ERROR_SAME_PERSON;
  //Get branchIds for accounts
  BranchID srcID = AccountNum_GetBranchID(srcAccount->accountNumber);
  BranchID dstID = AccountNum_GetBranchID(dstAccount->accountNumber);

  /*
   * If we are doing a transfer within the branch, we tell the Account module to
   * not bother updating the branch balance since the net change for the
   * branch is 0.
   */
  int updateBranch = !Account_IsSameBranch(srcAccountNum, dstAccountNum);

  //We cant always pick src to be the first one to lock. We need a comparator function to determine
  //which needs to be locked first, or else we will experience a deadlock. For accounts, we can compare
  //whatever we cant, i chose to compare account-s pointers and id-s for branches. I lock everything in passed
  //order (mut function explained in bank.c)
  if(updateBranch) {
    if(srcID < dstID) {
      mut(wait,4,&bank->branches[dstID].b_lock,&bank->branches[srcID].b_lock,&srcAccount->b_lock,&dstAccount->b_lock);
    }
    else {
      mut(wait,4,&bank->branches[srcID].b_lock,&bank->branches[dstID].b_lock,&dstAccount->b_lock,&srcAccount->b_lock);
    }
  } else {
    if(srcAccount > dstAccount) mut(wait,2,&srcAccount->b_lock,&dstAccount->b_lock);
    else mut(wait,2,&dstAccount->b_lock,&srcAccount->b_lock);
  } 
  
  //Account adjusts need to be atomic, for bank balance mismatch error
  pthread_mutex_lock(&bank->bank_bal);
  Account_Adjust(bank, srcAccount, -amount, updateBranch);
  Account_Adjust(bank, dstAccount, amount, updateBranch);
  pthread_mutex_unlock(&bank->bank_bal);

  //post locked mutexes, order of post does not matter
  mut(post,2,&srcAccount->b_lock,&dstAccount->b_lock);
  if(updateBranch) {
    mut(post,2,&bank->branches[srcID].b_lock,&bank->branches[dstID].b_lock);
  }
  return ERROR_SUCCESS;
}
