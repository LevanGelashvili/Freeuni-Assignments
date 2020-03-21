#ifndef _BANK_H
#define _BANK_H
#include  <semaphore.h>
#include "pthread.h"
#include "stdarg.h"



typedef struct Bank {
  unsigned int numberBranches;
  struct       Branch  *branches;
  struct       Report  *report;

  pthread_mutex_t bank_bal; //We use this lock to make updating branch balance atomic
  pthread_mutex_t rep_lock; //This lock makes sure each report happens atomically

  pthread_mutex_t check_lock; //Lock to check if arrived thread is the last to come
  pthread_mutex_t inc_lock; //Lock to increase and integer atomically
  sem_t lockIfArrived; //Semaphore (not mutex becase is will be locked several times) locking arrived threads
  int workersFinished; //Current number of arrived threads,
  int numWorkers; //Number of threads
  int favWorker; //Chooses which thead came last, to make him do the final report
} Bank;

#include "account.h"

int Bank_Balance(Bank *bank, AccountAmount *balance);

Bank *Bank_Init(int numBranches, int numAccounts, AccountAmount initAmount,
                AccountAmount reportingAmount,
                int numWorkers);

int Bank_Validate(Bank *bank);
int Bank_Compare(Bank *bank1, Bank *bank2);

void mut(int method(pthread_mutex_t* t), int argNum, ...);

#endif /* _BANK_H */
