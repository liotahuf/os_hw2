#include "ATM.h"

static int Bank_balance = 0;





int main(int argc, char* argv[])
{
	int ATM_num = atoi(argv[1]);
	if (ATM_num < 1)//not enought ATMs
	{
		cerr << "illegal arguments" << endl;
		exit(-1);
	}
	if (ATM_num != argc - 2) //number of ATMs and number of ATMs files do not match
	{
		cerr << "illegal number of arguments" << endl;
		exit(-1);
	}
	//initialize acc list locks
	if (pthread_mutex_init(&list_mutex_read, NULL) != 0)
	{
		cerr << strerror(errno) << endl;
		exit(-1);
	}
	if (pthread_mutex_init(&list_mutex_write, NULL) != 0)
	{
		cerr << strerror(errno) << endl;
		exit(-1);
	}
	
	//TODO other mutexes

	pthread_t* ATMs_threads = (pthread_t *)malloc(ATM_num * sizeof(pthread_t));
	if (ATMs_threads == NULL)
	{
		cerr << strerror(errno) << endl;
		exit(-1);
	}

	pATM_data ATM_threads_data;
	//allocate memory for ATM data array
	ATM_threads_data = (pATM_data)malloc(ATM_num * sizeof(ATM_data));
	if (ATM_threads_data == NULL) 
	{
		cerr << strerror(errno) << endl;
		free(ATMs_threads);
		exit(-1);
	}
	//update ATM data array for threads

	for (int i = 2; i <= ATM_num; i++)
	{
		ATM_threads_data[i].ATM_ID = i-1;
		ATM_threads_data[i].file= argv[i];

	}

	//create ATM threads
	for (int i = 0; i <= ATM_num; i++)
	{
		int rc;
		if ((rc = pthread_create(&ATMs_threads[i], NULL, ATMain, &ATM_threads_data[i])))
		{
			for(int j=)//TODO
			cerr << strerror(errno) << endl;
			exit(EXIT_FAILURE);
		}

	}



}