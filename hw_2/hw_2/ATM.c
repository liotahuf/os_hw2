#include "ATM.h"


//********************************************
// function name: ATMain
// Description: start routine of ATM threads,reads file and executes cmds
// Parameters: pointer to ATM struct(ID and ATM file)
// Returns: nothing(void)
//**************************************************************************************
void* ATMain(void* ptrATM)
{
	char line[MAX_LINE_SIZE];
	pATM curr_ATM = (pATM)ptrATM;
	
	char* args[MAX_ARG];
	int i = 0,num_arg = 0;
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
// function name: AccCreate
// Description: Creates account with account number and pwaasor
// Parameters: account number and pwaasor
// Returns: int -1 sucssess 0 failure
//**************************************************************************************
int AccCreate(int account_num, char password[PASSWORD_LENGTH + 1])
{
	//first,lock the list of accounts,so no one else can change it while we are trying to create account
	if (!pthread_mutex_lock(&accList_lock) !=0)
	{
		perror("unable to lock mutex\n");
		//exit(-1); TBD maybe this need to be thread exit
	}
	if (head == NULL)//acc list is empty
	{
		head = (pAccList)malloc(sizeof(Acclist));
		if (head == NULL)
		{

			perror("dynamic allocation failed\n");
			//exit(-1); TBD maybe this need to be thread exit
		}


		
	}

}
