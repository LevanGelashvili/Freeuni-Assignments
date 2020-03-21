#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>


#include "error.h"

#include "bank.h"
#include "branch.h"
#include "account.h"
#include "report.h"

/*
 * allocate the bank structure and initialize the branches.
 */
Bank*
Bank_Init(int numBranches, int numAccounts, AccountAmount initalAmount,
          AccountAmount reportingAmount,
          int numWorkers)
{

  Bank *bank = malloc(sizeof(Bank));

  if (bank == NULL) {
    return bank;
  } 

  Branch_Init(bank, numBranches, numAccounts, initalAmount);
  Report_Init(bank, reportingAmount, numWorkers);

  pthread_mutex_init(&bank->bank_bal,NULL);
  pthread_mutex_init(&bank->rep_lock,NULL);
  pthread_mutex_init(&bank->check_lock,NULL);
  pthread_mutex_init(&bank->inc_lock,NULL);
  sem_init(&bank->lockIfArrived,0,0);
  bank->workersFinished = 0;
  bank->numWorkers = numWorkers;
  return bank;
}

/*
 * get the balance of the entire bank by adding up all the balances in
 * each branch.
 * 
 * This is the method where we need to make checking bank balance atomic.
 */
int
Bank_Balance(Bank *bank, AccountAmount *balance)
{
  assert(bank->branches);
  pthread_mutex_lock(&bank->bank_bal); //lock
  AccountAmount bankTotal = 0;
  for (unsigned int branch = 0; branch < bank->numberBranches; branch++) {
    AccountAmount branchBalance;
    int err = Branch_Balance(bank,bank->branches[branch].branchID, &branchBalance);
    if (err < 0) {
      pthread_mutex_unlock(&bank->bank_bal); //post before return
      return err;
    }
    bankTotal += branchBalance;
  }

  *balance = bankTotal;
  pthread_mutex_unlock(&bank->bank_bal); //post
  return 0;
}

/*
 * tranverse and validate each branch.
 */
int
Bank_Validate(Bank *bank)
{
  assert(bank->branches);
  int err = 0;

  for (unsigned int branch = 0; branch < bank->numberBranches; branch++) {
    int berr = Branch_Validate(bank,bank->branches[branch].branchID);
    if (berr < 0) {
      err = berr;
    }
  }
  return err;
}

/*
 * compare the data inside two banks and see they are exactly the same;
 * it is called in BankTest.
 */
int
Bank_Compare(Bank *bank1, Bank *bank2)
{
  int err = 0;
  if (bank1->numberBranches != bank2->numberBranches) {
    fprintf(stderr, "Bank num branches mismatch\n");
    return -1;
  }

  for (unsigned int branch = 0; branch < bank1->numberBranches; branch++) {
    int berr = Branch_Compare(&bank1->branches[branch],
                              &bank2->branches[branch]);
    if (berr < 0) {
      err = berr;
    }
  }

  int cerr = Report_Compare(bank1, bank2);
  if (cerr < 0)
    err = cerr;

  return err;
}

/* This method takes 2 obilagory arguments, first is the method, passed by user,
  second is number of arguments, followed by ...
  Passed method is invoked for every argument, in this case, pthread lock or
  unlock to mutex pointers, in order to lock and unlock accounts and branches in teller.c
*/
void mut(int method(pthread_mutex_t* t), int argNum, ...) {
  va_list ls;
  va_start(ls,argNum);
  for (int i=0; i<argNum; i++) {
    method(va_arg(ls,pthread_mutex_t*));
  }
  va_end(ls);
}
