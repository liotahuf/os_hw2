#include "ATM.h"


//********************************************
// function name: ATMain
// Description: start routine of ATM threads,reads file and executes cmds
// Parameters: pointer to ATM struct(ID and ATM file)
// Returns: nothing(void)
//**************************************************************************************
void* ATMain(void* ptrATM_data)
{
	char line[MAX_LINE_SIZE];
	pATM_data curr_ATM = (pATM_data)ptrATM_data;


	char* args[MAX_ARG];
	int i = 0, num_arg = 0;
	char* delimiters = " \t\n";
	FILE* file = fopen(curr_ATM->file, "r");
	while (1) //read line from file
	{

		if (fgets(line, MAX_LINE_SIZE, file) == NULL)//EOF
		{
			break;
		}
		//parse line
		for (; i < MAX_ARG; i++)
		{
			args[i] = strtok(NULL, delimiters);
			if (args[i] != NULL)
				num_arg++;

		}
		char* cmd = args[0];
		//verify what cmd to do
		if (!strcmp(cmd, "O"))
		{
			//open account
			int account_num = atoi(args[1]);
			int initial_amount = atoi(args[3]);
			OpenAccount(account_num, args[2] , initial_amount, curr_ATM->ATM_ID);
			break;
		}
		else if (!strcmp(cmd, "D"))
		{
			//deposit 
		}
		else if (!strcmp(cmd, "W"))
		{
			//withdraw 
		}
		else if (!strcmp(cmd, "B"))
		{
			//balance check
		}
		else if (num_arg == 4)

		{
			// transfer between accounts

		}
		else
		{
			//ilegal cmd
		}
	}
	//kill thread

}

//********************************************
// function name: OpenAccount
// Description: Creates account with account number and pwaasor
// Parameters: account number and pwaasor
// Returns: int 0 sucssess -1 failure
//**************************************************************************************
int OpenAccount(int account_num, string password, int initial_amout, int ATM_ID)
{
	//first,lock the list of accounts,so no one else can change it while we are trying to create account
	pthread_mutex_lock(&acc_list_mutex_write);

	//first, verify if there alredy is an account with the same num
	if (SearchAccount(account_num) == -1) //no account with this account number
	{
		Account* new_account = new Account;
		new_account->account_num = account_num;
		new_account->balance = initial_amout;
		new_account->password = password;
		new_account->readers_cnt = 0;

		// initializing 2 mutexes for readers writers on account
		pthread_mutex_init(&new_account->balance_read_lock, NULL);
		

		pthread_mutex_init(&new_account->balance_write_lock, NULL);
		
		//add new account to account array
		account_list.push_back(*new_account);
		sleep(1); 

		//after one second, terminate action,write to log and release locks
		
		pthread_mutex_unlock(&acc_list_mutex_write);
		pthread_mutex_lock(&log_write_lock);
		outfile << ATM_ID << ": New account id is " << id<< " with password " << password<< " and initial balance " << initial_amout << endl;
		pthread_mutex_unlock(&log_write_lock);

		return 0;
	}
	else
	{
		pthread_mutex_unlock(&acc_list_mutex_write);
		pthread_mutex_lock(&log_write_lock);
		outfile <<"Error " << ATM_ID << ": Your transaction failed – account with the same id exists"<< endl;
		pthread_mutex_unlock(&log_write_lock);
		return -1;
	}
	

}


//********************************************
// function name: AccCreate
// Description: Creates account with account number and pwaasor
// Parameters: account number and pwaasor
// Returns: int 0 sucssess -1 failure
//**************************************************************************************
int Deposit(int account_num, string password, int initial_amout, int ATM_ID)
{
	//make readers/writers structure
	pthread_mutex_lock(&acc_list_mutex_read);
	list_read_count++;
	//now accounts list reader_lock locked
	if (list_read_count == 1) //if the first reader
	{
		//lock write_lock
		pthread_mutex_lock(&acc_list_mutex_write);
	}
	pthread_mutex_unlock(&acc_list_mutex_read);

	int i = SearchAccount(account_num);
	if (i == -1)//no account with this account num
	{
		outfile << "Error " << ATM_ID << ": Your transaction failed – account id "<< account_num<< " does not exist" << endl;
		return -1;
	}
	if (account_list[i].password == password)
	{

	}
}




// auxiliary functions

//********************************************
// function name: SearchAccount
// Description:Serach for account with account_num 
// Parameters: account number 
// Returns: index of the found account in account vector if it exists,else -1
//**************************************************************************************

int SearchAccount(int account_num) {	//  verify if there alredy is an account with the same num
	int found_accound = -1;
	int i =0;
	std::vector<Account>::iterator it;
	for (it = account_list.begin(); it != account_list.end(); it++,i++)
		if (it->account_num== id) {
			found_accound = i;
			break;
		}
	return found_accound;
}