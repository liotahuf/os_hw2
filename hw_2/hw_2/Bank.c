#include "ATM.h"

static int Bank_balance = 0;





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
	//initialize acc list lock
	if (pthread_mutex_init(&accList_lock, NULL) !=0)
	{
		perror("unable to initialize mutex\n");
		exit(-1);
	}
	//initialize acc list to null
	head = NULL;


	pthread_t* ATMs_threads = (pthread_t)malloc(ATM_num * sizeof(pthread_t));
	if (ATMs_threads == NULL)
	{
		perror("dynamic allocation failed\n");
		exit(-1);
	}

	//create ATM threads
	for (int i = 0; i <= ATM_num; i++)
	{
		rc = pthread_create(&)

	}



}