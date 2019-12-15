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
using std::deque;
ofstream outfile;

#define PASSWORD_LENGTH 4
#define MAX_ARG 4
#define MAX_LINE_SIZE 50
//-----globals-----
int num_of_threads;

typedef struct Account_ {

	string account_num;
	string password;
	int balance;
	//lock to deal wih paralelism
	pthread_mutex_t balance_read_lock;
	pthread_mutex_t balance_write_lock;
	int readers_cnt;

	
} Account;

vector <Account> account_list;
pthread_mutex_t list_mutex_read;
pthread_mutex_t list_mutex_write;
int list_read_count = 0;


typedef struct ATM_data_ {
	int ATM_ID;
	char* file;
} ATM_data, * pATM_data;


//functions
void* ATMain(void* ptrATM);


#endif