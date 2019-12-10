#include "ATM.h"

static int Bank_balance = 0;
pthread_mutex_t Bank_lock;
pthread_mutex_t Accounts_lock;
pthread_mutex_t Log_lock;




int main(int argc, char* argv[])
{
	int ATM_num = atoi(argv[1]);
	if (ATM_num < 1)//not enought ATMs
	{
		perror("illegal arguments\n");
		exit(-1);
	}
	if (ATM_num != argc - 2) //number of ATMs and number of ATMs files do not match
	{
		perror("illegal arguments\n");
		exit(-1);
	}



}