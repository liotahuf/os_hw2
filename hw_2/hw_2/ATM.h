#ifndef _ATM_H
#define _ATM_H
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cmath>
#include <pthread.h>
#include<vector>
#include<algorithm>
#include<string>

using namespace std;
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::stringstream;

#define PASSWORD_LENGTH 4
#define MAX_ARG 4
#define MAX_LINE_SIZE 50
//-----globals-----
int num_of_threads;
typedef struct account_ {
	int account_num;
	char password[PASSWORD_LENGTH + 1];
	int balance;
	pthread_mutex_t balance_read_lock;
	pthread_mutex_t balance_write_lock;
}account;

typedef struct accounts_list
{
	account curr_account;
	struct accounts_list* next_account;


} Acclist, * pAccList;

pthread_mutex_t accList_lock;
pAccList head;

typedef struct ATM_ {
	int ATM_ID;
	char* file;
} ATM, * pATM;

#endif