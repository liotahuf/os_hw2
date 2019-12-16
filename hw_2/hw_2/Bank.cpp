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
		cerr << "error: illegal number of arguments" << endl;
		exit(-1);
	}

	//open/create file for log
	outfile.open("log.txt", ios::out | ios::trunc);
	if (outfile.fail()) {
		cerr << "error: open/create log file failed" << endl;
		exit (-1);
	}

	//initialize acc list locks
	if (pthread_mutex_init(&lacc_list_mutex_read, NULL) != 0)
	{
		cerr << "error: mutex initialization error" << endl;
		exit(-1);
	}
	if (pthread_mutex_init(&acc_list_mutex_read, NULL) != 0)
	{
		cerr << "error: mutex initialization error" << endl;
		exit(-1);
	}
	
	//TODO other mutexes

	pthread_t* ATMs_threads = new pthread_t[ATM_num];

	pATM_data ATM_threads_data;
	//allocate memory for ATM data array
	ATM_threads_data = new ATM_data[ATM_num];

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
				
			delete[] ATM_threads_data;
			delete[] ATMs_threads;
			cerr << "error : thread creation failed" << endl;
			exit(-1);
		}

	}


	for (int j = 0; j < ATM_num; j++)
	{
		void** retval;
		pthread_join(&ATMs_threads[j], retval);
		if (retval != NULL)//some thread return becasue of a failure,so end the program
		{
			cerr << "error : fail in a thread " << endl;
			exit(-1);
		}
	}

	//close log file
	outfile.close();
	if (outfile.fail()) {
		cerr << "error : failed to close log file" << endl;
		return 1;
	}
	return 0;


}