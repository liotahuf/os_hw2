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

extern ofstream outfile;

#define PASSWORD_LENGTH 4
#define MAX_ARG 4
#define MAX_LINE_SIZE 50 
/*longest line = bank transfers 2 accounts(each has at most 10 digits) ,password(4 digits) and balance(at most 10 digits) 
so this number should be bigger than max line width*/


//-----globals-----
extern int num_of_threads;

typedef struct Account_ {

	int account_num;
	string password;
	int balance;
	//lock to deal wih paralelism
	pthread_mutex_t balance_read_lock;
	pthread_mutex_t balance_write_lock;
	int readers_cnt;

	
} Account;

// accounts array and its locks
extern vector <Account> account_list;
extern pthread_mutex_t acc_list_mutex_read;
extern pthread_mutex_t acc_list_mutex_write;
extern int  list_read_count;


typedef struct ATM_data_ {
	int ATM_ID;
	char* file;
} ATM_data, * pATM_data;

//log file locks

extern pthread_mutex_t log_mutex_read;
extern pthread_mutex_t log_mutex_write;
extern int log_read_count ;

//functions
void* ATMain(void* ptrATM);
int OpenAccount(int account_num, string password, int initial_amout, int ATM_ID);
int Deposit(int account_num, string password, int amount, int ATM_ID);
int Withdraw(int account_num, string password, int amount, int ATM_ID);
int BalanceCheck(int account_num, string password, int ATM_ID);
int Transfer(int account_num_src, string password, int account_num_dst, int amount, int ATM_ID);
int SearchAccount(int account_num);

#endif
