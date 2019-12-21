#include "ATM.h"

ofstream outfile;
int num_of_threads;
vector <Account> account_list;
pthread_mutex_t acc_list_mutex_read;
pthread_mutex_t acc_list_mutex_write;
int list_read_count = 0;
pthread_mutex_t log_mutex_read;
pthread_mutex_t log_mutex_write;
int log_read_count = 0;

//********************************************
// function name: ATMain
// Description: start routine of ATM threads,reads file and executes cmds
// Parameters: pointer to ATM struct(ID and ATM file)
// Returns: nothing(void)
//**************************************************************************************
void* ATMain(void* ptrATM_data)
{

	pATM_data curr_ATM = (pATM_data)ptrATM_data;

	//int i = 0, num_arg = 0;
	ifstream file(curr_ATM->file); //input file stream
	string line;
	if (!file || !file.good()) {
		// File doesn't exist or some other error
		cerr << "File not found" << endl;
		return 0;
	}
	while (getline(file, line))
	{

		usleep(100000);
		stringstream ss(line);
		char cmd; // read (R) or write (W)
		string account_num_s;
		string password;
		if (!(ss >> cmd >> account_num_s >> password))
		{
			// Operation appears in an Invalid format
			cerr << "Command Format error" << endl;
			return 0;
		}
		if (!strcmp(&cmd, "O"))
		{
			//open account

			string initial_amount_s;
			if (!(ss >> initial_amount_s)) {
				// Operation appears in an Invalid format
				cerr << "Command Format error" << endl;
				return 0;
			}
			int account_num = atoi(account_num_s.c_str());
			int initial_amount = atoi(initial_amount_s.c_str());
			OpenAccount(account_num, password, initial_amount, curr_ATM->ATM_ID);
			continue;
		}
		else if (!strcmp(&cmd, "D"))
		{
			string amount_s;
			if (!(ss >> amount_s))
			{
				// Operation appears in an Invalid format
				cerr << "Command Format error" << endl;
				return 0;
			}
			int account_num = atoi(account_num_s.c_str());

			int amount = atoi(amount_s.c_str());
			Deposit(account_num, password, amount, curr_ATM->ATM_ID);
			continue;
		}
		else if (!strcmp(&cmd, "W"))
		{
			string amount_s;
			if (!(ss >> amount_s))
			{
				// Operation appears in an Invalid format
				cerr << "Command Format error" << endl;
				return 0;
			}
			int account_num = atoi(account_num_s.c_str());

			int amount = atoi(amount_s.c_str());
			Withdraw(account_num, password, amount, curr_ATM->ATM_ID);
			continue;
		}
		else if (!strcmp(&cmd, "B"))
		{
			int account_num = atoi(account_num_s.c_str());

			BalanceCheck(account_num, password,  curr_ATM->ATM_ID);
			continue;
		}
		else if (!strcmp(&cmd, "T"))

		{
			int account_num_src = atoi(account_num_s.c_str());


			string account_num_dst_s;
			if (!(ss >> account_num_dst_s))
			{
				// Operation appears in an Invalid format
				cerr << "Command Format error" << endl;
				return 0;
			}
			int account_num_dst = atoi(account_num_dst_s.c_str());

			string amount_s;
			if (!(ss >> amount_s))
			{
				// Operation appears in an Invalid format
				cerr << "Command Format error" << endl;
				return 0;
			}
			int amount = atoi(amount_s.c_str());
			Transfer(account_num_src, password, account_num_dst, amount, curr_ATM->ATM_ID);
			continue;

		}
		else
		{
			pthread_mutex_lock(&log_mutex_write);
			outfile << "Error " << curr_ATM->ATM_ID << ": Illegal command" << endl;
			pthread_mutex_unlock(&log_mutex_write);
		}
	}
	//kill thread
	file.close();
	return 0;

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
	int account_idx = SearchAccount(account_num);
	if (account_idx == -1) //no account with this account number
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
		//lock account so no other atm can do anything to it until we write to log
		pthread_mutex_lock(&account_list[account_idx].balance_write_lock);
		sleep(1); 

		//after one second, terminate action,write to log and release locks
		
		pthread_mutex_unlock(&acc_list_mutex_write);
		pthread_mutex_lock(&log_mutex_write);
		outfile << ATM_ID<< ": New account id is " << account_num<< " with password " << password<< " and initial balance " << initial_amout << endl;
		pthread_mutex_unlock(&log_mutex_write);
		pthread_mutex_unlock(&account_list[account_idx].balance_write_lock);

		return 0;
	}
	else
	{
		pthread_mutex_unlock(&acc_list_mutex_write);
		pthread_mutex_lock(&log_mutex_write);
		outfile <<"Error " << ATM_ID << ": Your transaction failed - account with the same id exists"<< endl;
		pthread_mutex_unlock(&log_mutex_write);
		return -1;
	}
	

}


//********************************************
// function name: Deposit
// Description: Deposits amount in account with account number account_num
// Parameters: account_num,password, amount,atm id
// Returns: int 0 sucssess -1 failure
//**************************************************************************************
int Deposit(int account_num, string password, int amount, int ATM_ID)
{
	int return_status;
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

	int account_idx = SearchAccount(account_num);
	//lock account to write balanceso no other atm can perform action until this ended
	if (account_idx != -1)
	{
		pthread_mutex_lock(&account_list[account_idx].balance_write_lock);
	}
		

	
	if (account_idx == -1)//no account with this account num
	{
		pthread_mutex_lock(&log_mutex_write);
		outfile << "Error " << ATM_ID << ": Your transaction failed - account id "<< account_num<< " does not exist" << endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = -1;

		
	}else if (account_list[account_idx].password != password)
	{
		pthread_mutex_lock(&log_mutex_write);
		outfile << "Error " << ATM_ID << ": Your transaction failed - password for account"<< account_list[account_idx].account_num <<" is incorrect" << endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = -1;
		
	}
	else
	{

		account_list[account_idx].balance += amount;
		sleep(1);
		pthread_mutex_lock(&log_mutex_write);
		outfile << ATM_ID << ": Account " << account_list[account_idx].account_num << " new balance is " << account_list[account_idx].balance << " after " << amount << " $ was deposited" << endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = 0;
	}

	//release account lock
	if (account_idx != -1)
	{
		pthread_mutex_unlock(&account_list[account_idx].balance_write_lock);
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
	return return_status;
}


//********************************************
// function name: Withdraw
// Description: Withdram amount form account with account_num
// Parameters: account_num,password,amiunt,atm id
// Returns: int 0 sucssess -1 failure
//**************************************************************************************
int Withdraw(int account_num, string password, int amount, int ATM_ID)
{
	int return_status ;
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

	int account_idx = SearchAccount(account_num);

	//lock account to write balanceso no other atm can perform action until this ended
	if (account_idx != -1)
	{
		pthread_mutex_lock(&account_list[account_idx].balance_write_lock);
	}

	
	if (account_idx == -1)//no account with this account num
	{
		pthread_mutex_lock(&log_mutex_write);
		outfile << "Error " << ATM_ID << ": Your transaction failed - account id " << account_num << " does not exist" << endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = -1;
	}
	else if (account_list[account_idx].password != password)
	{
		pthread_mutex_lock(&log_mutex_write);
		outfile << "Error " << ATM_ID << ": Your transaction failed - password for account" << account_list[account_idx].account_num << " is incorrect" << endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = -1;
	}
	else if(account_list[account_idx].balance < amount)
	{
		pthread_mutex_lock(&log_mutex_write);
		outfile << "Error " << ATM_ID << ": Your transaction failed - account id " << account_list[account_idx].account_num << " balance is lower than " << amount<<  endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = -1;
	}
	else //withdraw can be done
	{
		account_list[account_idx].balance -= amount;
		sleep(1);
		pthread_mutex_lock(&log_mutex_write);
		outfile << ATM_ID << ": Account " << account_list[account_idx].account_num << " new balance is " << account_list[account_idx].balance << " after " << amount << " $ was withdrew" << endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = 0;
	}
	//release account lock
	if (account_idx != -1)
	{
		pthread_mutex_unlock(&account_list[account_idx].balance_write_lock);
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
	return return_status;
}


//********************************************
// function name: BalanceCheck
// Description: Check balance of acount with account_num
// Parameters: account_num,password,atm id
// Returns: int 0 sucssess -1 failure
//**************************************************************************************
int BalanceCheck(int account_num, string password, int ATM_ID)
{
	int return_status;
	//make readers/writers structure for accounts array
	pthread_mutex_lock(&acc_list_mutex_read);
	list_read_count++;
	//now accounts list reader_lock locked
	if (list_read_count == 1) //if the first reader
	{
		//lock write_lock
		pthread_mutex_lock(&acc_list_mutex_write);
	}
	pthread_mutex_unlock(&acc_list_mutex_read);

	
	int account_idx = SearchAccount(account_num);
	if (account_idx == -1)
	{
		pthread_mutex_lock(&log_mutex_write);
		outfile << "Error " << ATM_ID << ": Your transaction failed - account id " << account_num << " does not exist" << endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = -1;
	}
	else if (account_list[account_idx].password != password)
	{
		pthread_mutex_lock(&log_mutex_write);
		outfile << "Error " << ATM_ID << ": Your transaction failed - password for account" << account_list[account_idx].account_num << " is incorrect" << endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = -1;
	}
	else 
	{
		//make readers/writers structure for account
		pthread_mutex_lock(&account_list[account_idx].balance_read_lock);
		account_list[account_idx].readers_cnt++;
		if (account_list[account_idx].readers_cnt == 1)//first reader of account
		{
			pthread_mutex_lock(&account_list[account_idx].balance_write_lock);
		}
		pthread_mutex_unlock(&account_list[account_idx].balance_read_lock);

		//read balance
		int balance = account_list[account_idx].balance;
		sleep(1);
		pthread_mutex_lock(&log_mutex_write);
		outfile <<  ATM_ID << ": Account " << account_num << " balance is "<< balance << endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = 0;

		pthread_mutex_lock(&account_list[account_idx].balance_read_lock);
		account_list[account_idx].readers_cnt--;
		if (account_list[account_idx].readers_cnt == 0)//first reader of account
		{
			pthread_mutex_unlock(&account_list[account_idx].balance_write_lock);
		}
		pthread_mutex_unlock(&account_list[account_idx].balance_read_lock);
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
	return return_status;
	
}

//********************************************
// function name: Transfer
// Description: Transfer amount form account_num_src to account_num_dst
// Parameters: account_num,password,atm id
// Returns: int 0 sucssess -1 failure
//**************************************************************************************
int Transfer(int account_num_src, string password,int account_num_dst,int amount ,int ATM_ID)
{
	int return_status;
	//make readers/writers structure for accounts array
	pthread_mutex_lock(&acc_list_mutex_read);
	list_read_count++;
	//now accounts list reader_lock locked
	if (list_read_count == 1) //if the first reader
	{
		//lock write_lock
		pthread_mutex_lock(&acc_list_mutex_write);
	}
	pthread_mutex_unlock(&acc_list_mutex_read);

	int account_src_idx = SearchAccount(account_num_src);
	int account_dst_idx = SearchAccount(account_num_dst);
	if (account_src_idx ==-1)
	{
		pthread_mutex_lock(&log_mutex_write);
		outfile << "Error " << ATM_ID << ": Your transaction failed - account id " << account_num_src << " does not exist" << endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = -1;
	}
	else if (account_dst_idx == -1)
	{
		pthread_mutex_lock(&log_mutex_write);
		outfile << "Error " << ATM_ID << ": Your transaction failed - account id " << account_num_dst << " does not exist" << endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = -1;
	}
	else if (account_list[account_src_idx].password != password)
	{
		pthread_mutex_lock(&log_mutex_write);
		outfile << "Error " << ATM_ID << ": Your transaction failed - password for account" << account_list[account_src_idx].account_num << " is incorrect" << endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = -1;

	}
	else if (account_src_idx == account_dst_idx)
	{
		pthread_mutex_lock(&log_mutex_write);
		outfile << "Error " << ATM_ID << ": Your transaction failed - cannot transfer to same account" << endl;
		pthread_mutex_unlock(&log_mutex_write);
		return_status = -1;
		
	}
	else
	{
		//make sure all account lock is in the same order
		int min_idx, max_idx;
		if (account_src_idx < account_dst_idx)
		{
			min_idx = account_src_idx;
			max_idx = account_dst_idx;

		}
		else
		{
			min_idx = account_dst_idx;
			max_idx = account_src_idx;
		}
		pthread_mutex_lock(&account_list[min_idx].balance_write_lock);
		pthread_mutex_lock(&account_list[max_idx].balance_write_lock);

		if (account_list[account_src_idx].balance< amount)
		{
			pthread_mutex_lock(&log_mutex_write);
			outfile << "Error " << ATM_ID << ": Your transaction failed - account id " << account_list[account_src_idx].account_num << " balance is lower than " << amount<< endl;
			pthread_mutex_unlock(&log_mutex_write);
			return_status = -1;
		}
		else
		{
			account_list[account_src_idx].balance -= amount;
			account_list[account_dst_idx].balance += amount;
			sleep(1);
			pthread_mutex_lock(&log_mutex_write);
			outfile << ATM_ID << ": Transfer " << amount<< " from account "<< account_list[account_src_idx].account_num << " to account "<< account_list[account_dst_idx].account_num << " new account balance is " << account_list[account_src_idx].balance <<	" new target account balance is " << account_list[account_dst_idx].balance << endl;
			pthread_mutex_unlock(&log_mutex_write);
			return_status = 0;
		
		}
		pthread_mutex_unlock(&account_list[min_idx].balance_write_lock);
		pthread_mutex_unlock(&account_list[max_idx].balance_write_lock);

	}
	// reduce  numberof readers of accounts list
	pthread_mutex_lock(&acc_list_mutex_read);
	list_read_count--;
	if (list_read_count == 0) //if no more readers
	{
		//unlock write_lock
		pthread_mutex_unlock(&acc_list_mutex_write);
	}
	pthread_mutex_unlock(&acc_list_mutex_read);
	return return_status;
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
	if(account_list.size() !=0)
	{
		for (it = account_list.begin(); it != account_list.end(); it++,i++)
			if ((it->account_num) == account_num)
			{
				found_accound = i;
				break;
			}
	}

	return found_accound;
}
