#ifndef _ATM_H
#define _ATM_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#define PASSWORD_LENGTH 4
#define MAX_ARG 4
#define MAX_LINE_SIZE 50
//-----globals-----
int num_of_threads;
typedef struct account_ {
	int account_num;
	char password[PASSWORD_LENGTH+1];
	int balance;
	pthread_mutex_t balance_read_lock;
	pthread_mutex_t balance_write_lock;
}account;

typedef struct accounts_list
{
	account curr_account;
	struct accounts_list* next_account;
	

} Acclist,*pAccList;

pthread_mutex_t accList_lock;
pAccList head;

typedef struct ATM_ {
	int ATM_ID;
	char* file;
} ATM, *pATM;

#endif