#include "ATM.h"



//globals for bank
static int bank_balance = 0;
pthread_mutex_t bank_balance_mutex_read;
pthread_mutex_t bank_balance_mutex_write;
static int bank_read_count = 0;
int ATM_end;

//declare bank treads functions
void* comission_func(void* arg);
void* print_status_func(void* arg);

/********************************************
// function name: main
// Description: main bank function,creates and mnage all threads
// Parameters: argc,argv - from command line
// Returns: int 0 if sucssess -1 failure
//**************************************************************************************/

int main(int argc, char* argv[])
{
	ATM_end = 0;
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
	if (pthread_mutex_init(&acc_list_mutex_read, NULL) != 0)
	{
		cerr << "error: mutex initialization error" << endl;
		exit(-1);
	}
	if (pthread_mutex_init(&acc_list_mutex_read, NULL) != 0)
	{
		cerr << "error: mutex initialization error" << endl;
		exit(-1);
	}
	
	//initialize bank balance
		if (pthread_mutex_init(&bank_balance_mutex_read, NULL) != 0)
		{
			cerr << "error: mutex initialization error" << endl;
			exit(-1);
		}
		if (pthread_mutex_init(&bank_balance_mutex_write, NULL) != 0)
		{
			cerr << "error: mutex initialization error" << endl;
			exit(-1);
		}

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

	//comission thread - take commition from each account
	pthread_t Comission_thread;
	int rc;
	if ((rc = pthread_create(&Comission_thread, NULL, comission_func, NULL)))
	{
		delete[] ATM_threads_data;
		delete[] ATMs_threads;
		cerr << "error : thread creation failed" << endl;
		exit(-1);
	}

	//prinf status thread - prints banks status
	pthread_t Print_thread;

	if ((rc = pthread_create(&Print_thread, NULL, print_status_func, NULL)))
	{
		delete[] ATM_threads_data;
		delete[] ATMs_threads;
		cerr << "error : thread creation failed" << endl;
		exit(-1);
	}
	

	for (int j = 0; j < ATM_num; j++)
	{
		void** retval;
		pthread_join(ATMs_threads[j], retval);
		if (retval != NULL)//some thread return becasue of a failure,so end the program
		{
			cerr << "error : fail in a thread " << endl;
			exit(-1);
		}
	}
	//wait for ATM to end
	ATM_end = 1;


	void** retval2;
	pthread_join(Comission_thread, retval2);
	if (retval2 != NULL)//some thread return becasue of a failure,so end the program
	{
		cerr << "error : fail in a thread " << endl;
		exit(-1);
	}
	
	void** retval3;
	pthread_join(Print_thread, retval3);
	if (retval3 != NULL)//some thread return becasue of a failure,so end the program
	{
		cerr << "error : fail in a thread " << endl;
		exit(-1);
	}
	

	//close log file
	outfile.close();
	if (outfile.fail()) 
	{
		cerr << "error : failed to close log file" << endl;
		return 1;
	}

	// destroing the accounts' mutexes
	std::vector<Account>::iterator it;
	for (it = account_list.begin(); it != account_list.end(); it++) {
		if (pthread_mutex_destroy(&it->balance_read_lock) != 0) {
			cerr << strerror(errno) << endl;
			exit(EXIT_FAILURE);
		}
		if (pthread_mutex_destroy(&it->balance_write_lock) != 0) {
			cerr << strerror(errno) << endl;
			exit(EXIT_FAILURE);
		}
	}

	// destroing the vectors + bank balance + log file mutexes
	if (pthread_mutex_destroy(&acc_list_mutex_read) != 0) {
		cerr << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	if (pthread_mutex_destroy(&acc_list_mutex_write) != 0) {
		cerr << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	if (pthread_mutex_destroy(&bank_balance_mutex_read) != 0) {
		cerr << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	if (pthread_mutex_destroy(&bank_balance_mutex_write) != 0) {
		cerr << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	if (pthread_mutex_destroy(&log_mutex_read) != 0) {
		cerr << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	if (pthread_mutex_destroy(&log_mutex_write) != 0) {
		cerr << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}

	// free the memory we allocated before
	delete[] ATM_threads_data;
	delete[] ATMs_threads;


	return 0;


}


/********************************************
// function name: comission_func
// Description: Charge commisiion from every account every 3 secons 
// Parameters: void arg(not used)
// Returns: void(nothing)
//**************************************************************************************/
void* comission_func(void* arg)
{
	std::vector<Account>::iterator it;
	while (1)
	{
		sleep(3);
		if (ATM_end == true)
			break;
		//make readers/writers structure on accounts array
		pthread_mutex_lock(&acc_list_mutex_read);
		list_read_count++;
		//now accounts list reader_lock locked
		if (list_read_count == 1) //if the first reader
		{
			//lock write_lock
			pthread_mutex_lock(&acc_list_mutex_write);
		}
		pthread_mutex_unlock(&acc_list_mutex_read);

		if (account_list.size() != 0)
		{
			srand(time(NULL));
			double range = (0.04 - 0.02);
			double div = RAND_MAX / range;
			double commission_percentage= 0.02 + (rand() / div);
			
			for (it = account_list.begin(); it != account_list.end(); it++)
			{
				//lock accoutn because we want to wrtie on it
				pthread_mutex_lock(&(it->balance_write_lock));
				double comission = commission_percentage * (double)it->balance;
				int commission_rounded = int(comission);
				
				it->balance = it->balance - commission_rounded;
				
				pthread_mutex_lock(&log_mutex_write);
				outfile << "Bank: commissions of " << commission_percentage*100 << " % were charged, the bank gained " << commission_rounded << " $ from account " << it->account_num << endl;
				pthread_mutex_unlock(&log_mutex_write);
				pthread_mutex_unlock(&it->balance_write_lock);

				// readers-writers on the current bank balance
				pthread_mutex_lock(&bank_balance_mutex_write);
				bank_balance = bank_balance + commission_rounded;
				pthread_mutex_unlock(&bank_balance_mutex_write);

			}
		}
		//reduce  numberof readers of accounts list
		pthread_mutex_lock(&acc_list_mutex_read);
		list_read_count--;
		if (list_read_count == 0) //if no more readers
		{
			//unlock write_lock
			pthread_mutex_unlock(&acc_list_mutex_write);
		}
		pthread_mutex_unlock(&acc_list_mutex_read);

	}
	

}

/********************************************
// function name: print_status_func
// Description: Prints banks stuts every 0.5 seconds, accounts balance,number and passwor
// Parameters: void arg(not used)
// Returns: void(nothing)
//**************************************************************************************/
void* print_status_func(void* arg)
{
	std::vector<Account>::iterator it;
	while (1)
	{
		sleep(0.5);
		if (ATM_end == true)
		{
			break;
		}

		printf("\033[2J");
		printf("\033[1;1H");
		printf("Current Bank Status\n");

		if (account_list.size() != 0)
		{
			//make readers/writers structure on accounts array
			pthread_mutex_lock(&acc_list_mutex_read);
			list_read_count++;
			//now accounts list reader_lock locked
			if (list_read_count == 1) //if the first reader
			{
				//lock write_lock
				pthread_mutex_lock(&acc_list_mutex_write);
			}
			pthread_mutex_unlock(&acc_list_mutex_read);

			for (it = account_list.begin(); it != account_list.end(); it++)
			{
				//make readers/writers structure for account
				pthread_mutex_lock(&it->balance_read_lock);
				it->readers_cnt++;
				if (it->readers_cnt == 1)//first reader of account
				{
					pthread_mutex_lock(&it->balance_write_lock);
				}
				pthread_mutex_unlock(&it->balance_read_lock);

				cout << "Account " << it->account_num << ": Balance - " << it->balance << " $, Account Password - " << it->password << endl;

				//reduce reader os account
				pthread_mutex_unlock(&it->balance_read_lock);
				it->readers_cnt--;
				if (it->readers_cnt == 0)//last reader of account
				{
					pthread_mutex_unlock(&it->balance_write_lock);
				}
				pthread_mutex_unlock(&it->balance_read_lock);

			}

			//reduce  numberof readers of accounts list
			pthread_mutex_lock(&acc_list_mutex_read);
			list_read_count--;
			if (list_read_count == 0) //if no more readers
			{
				//unlock write_lock
				pthread_mutex_unlock(&acc_list_mutex_write);
			}
			pthread_mutex_unlock(&acc_list_mutex_read);

		}

		pthread_mutex_lock(&bank_balance_mutex_read);
		bank_read_count++;
		if (bank_read_count == 1)
		{
			pthread_mutex_lock(&bank_balance_mutex_write);
		}
		pthread_mutex_unlock(&bank_balance_mutex_read);
		printf("The Bank has %d $\n", bank_balance);
		pthread_mutex_lock(&bank_balance_mutex_read);
		bank_read_count--;
		if (bank_read_count == 0)
		{
			pthread_mutex_unlock(&bank_balance_mutex_write);
		}
		pthread_mutex_unlock(&bank_balance_mutex_read);
	}
	pthread_exit(NULL);
}
