/****************************
Task 5
Tamara Pando 
Semaphores
****************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>		//..
#include <sys/ipc.h>		// for semaphores segmet(), semctl(), semop()
#include <sys/sem.h>		//.. 
#include <sys/stat.h> 
#include <sys/wait.h>
#include <linux/limits.h>

#define BUFF 64         // buffer 
#define KEY1 1100
#define KEY2 1111
//**************************************


int count = 0;      // union for semaphores 
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};

int sem1, sem2;		// declaring semaphores 

//************************************* functions used 
int SEM_ON(int sem_id, int sem_val);
int SEM_OFF(int sem_id);
int P(int sem_id);
int V(int sem_id);
void getUserIn(char *userIn);
void atExit();
void exitingShell(char **history, int len);
void treeCommand();
//*************************************************************

// ******************************************************************************************************************MAIN 
int main()
{
	int pid;						// Process ID after fork call
	int status;						// Exit status of child process
    int loops = 0; 
    char userIn[BUFF];  
    char *lists[20];
	
    
        while(1)
        {
            //printf("Dad Pid: %d\n",getpid()); 
            if(loops == 0)
            {
            system("clear"); //clear screen 
            }

            char* userName = getenv("USER"); 
            printf("%s@shell ~> ", userName); 
            getUserIn(userIn); 
            lists[loops] = strdup(userIn); 

            if(strcmp(userIn, "exit*") == 0) {
            exitingShell(lists,loops);
            exit(0); 
            }
            else if(strcmp(userIn, "t") == 0){
                treeCommand();   
            }
		
	    loops++; 
    }		  
}


/* exitingShell()*
  -> prints a list of the last 4 commands of the terminal
  -> a detail list of the current directory like ls -l
  -> wait for the user to hit return to return the control to the original shell 
*/
//******************* EXITINGSHELL function *************************************************************************************************
void exitingShell(char **history, int len ){

    int status; 
    pid_t pid;
    int ret; 
    pid = fork(); 

    if(pid == 0)
    {
        //=====================================================CODE CHILD 1 
        printf("1 PPID: %d, PID: %d\n", getppid(), getpid()); 
        printf("\nPress ENTER to exit.\n"); 
        
        if(len <= 4){                           // loop for keeping count of commands if <= 4
            printf("\nCommands used: \n"); 
            for(int i = 0; i < len; i++)
                printf("   %s \n", history[i]); 
            if(len == 0)
            printf("exit* \n"); 
        }
        else{
            printf("\nLast four commands used: \n");        // loop for printing last 4 commands
            for(int i = (len-4); i < len; i++) 
                printf("   %s \n", history[i]); 
        }
        puts("\n");  
        exit(0); 

        // =============================================== END CHILD 1 

    }else if(pid < 0)
    {
        perror("Error creating child process\n"); 
        exit(EXIT_FAILURE); 
    }else
    {
        pid = fork(); 
        if(pid == 0)
        { //=========================================================== CODE CHILD 2    
            printf("2 PPID: %d, PID: %d\n", getppid(), getpid()); 
             
            char* prog1[] = { "ls", "-l",0};
            ret = execvp(prog1[0], prog1);                          // executing ls -F 
            if(ret == -1)
            {
                perror("exec error prog 1"); 
                exit(0); 
            }
                //===================================================== END CHILD 2
        }
        else if(pid < 0)
        {
            perror("Error creating child process\n"); 
            exit(EXIT_FAILURE);
            }
            else
            {
                atExit(); 
                pid = wait(&status);
				printf("Process(pid = %d) exited with the status %d. \n", pid, status);
			
				pid = wait(&status);
				printf("Process(pid = %d) exited with the status %d. \n", pid, status);  
            }    
    } 
    
}

/*  atExit will exit out the project when Enter is clicked, also the ... process.
*/
//******************* ATEXIT function *************************************************************************************************
void atExit()
{
    char ch; 
    ch = fgetc(stdin); 

    if(ch == 0x0A){
      printf("\nExiting shell");
        for (int i = 0; i < 7; i++) 
        {
            usleep(500 * 1000);
            fputc('.', stdout);
            fflush(stdout);
        }
        printf("\n\n");
    }
}



// SEM_ON function to set semaphores 
//******************* SEM_ON function *************************************************************************************************
int SEM_ON(int sem_id, int sem_val)
{
    union semun sem_union; 
    sem_union.val = sem_val; 
    return semctl(sem_id, 0, SETVAL, sem_union); 
}

// SEM_OFF function to unlink semaphores 
// takes semaphore as a parameter 
//******************* SEM_OFF function *************************************************************************************************
int SEM_OFF(int sem_id)
{
    return semctl(sem_id, 0, IPC_RMID); 
}

// P function for waiting, decrements semaphore
// takes semaphore as a parameter 
//******************* P function *************************************************************************************************
int P(int sem_id)
{
    struct sembuf sem_buf; 

    sem_buf.sem_num = 0; 
    sem_buf.sem_op = -1; 
    sem_buf.sem_flg = SEM_UNDO; 
    return semop(sem_id, &sem_buf, 1); 
}

// V function for waiting, increments sempahore 
// takes semaphore as a parameter 
//******************* V function *************************************************************************************************
int V(int sem_id)
{
    struct sembuf sem_buf; 

    sem_buf.sem_num = 0; 
    sem_buf.sem_op = 1; 
    sem_buf.sem_flg = SEM_UNDO; 
    return semop(sem_id, &sem_buf, 1); 
}

//******************* GETUSERIN function *************************************************************************************************
void getUserIn(char *userIn)
{
    fgets(userIn, BUFF, stdin); 
    int len; 
    len = (strlen(userIn)-1); 

    if(userIn[len]=='\n')  //fgets appends \n
        userIn[len] = '\0';
}

//******************* TREECOMMAND function *************************************************************************************************
void treeCommand()
{
    FILE *fp1, *fp2, *fp3;			/* File Pointers */
//****************************************** SETTING SEMAPHORES 
    sem1 = semget(KEY1, 1, IPC_CREAT | 0666); 
    if(sem1 < 0)
    {
        perror("ERROR: semget sem1\n"); 
        exit(EXIT_FAILURE); 
    }
    sem2 = semget(KEY2, 1, IPC_CREAT | 0666); 
    if(sem2 < 0)
    {
        perror("ERROR: segmet sem2\n");
        exit(EXIT_FAILURE); 
    }
   
	SEM_ON(sem1,1); 		//INITIALIZING SEMAPHORES 
    SEM_ON(sem2,0);  

    int status; 
    pid_t pid;
    int ret; 

	if ((pid = fork()) == -1)
	{
			//Fork failed!
		perror("fork");
		exit(1);
	}

	if (pid == 0)
	{   //==================================================== CHILD 1 PROCESS
                
        //printf("1 PPID: %d, PID: %d\n", getppid(), getpid()); 
        P(sem1); 
        printf("Two\n");
            
        char* prog1[] = { "mkdir", "Dir0", 0};
        ret = execvp(prog1[0], prog1);    // executing ls -F 
        if(ret == -1)
        {
            perror("exec error prog 1"); 
            exit(0); 
        }
        V(sem2); 
        exit(0);                                            
		//==================================================== END CHILD 1 PROCESS 
	}
    else
	{
		//Parent Process. Fork off one more child process.
		if ((pid = fork()) == -1) 
		{
				//fork failed!
			perror("fork");
			exit(1);
		}
		if (pid == 0)
		{  //==================================================== CHILD 2 PROCESS 
            //printf("2 PPID: %d, PID: %d\n", getppid(), getpid());
            P(sem2); 
            printf("Three\n"); 
                
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) 
            {
                printf("Current working dir: %s\n", cwd);
            } 
            else 
            {
                perror("getcwd() error");
                exit(0); 
            }
            strcat(cwd, "/Dir0"); 
        
            if (chdir(cwd) != 0)  
                perror("chdir() to /Dir0 failed"); 

            char a[PATH_MAX];
            if (getcwd(a, sizeof(a)) != NULL) 
            {
                printf("Current working dir: %s\n", a);
            } 
            else 
            {
                perror("getcwd() error");
                exit(0); 
            }

            fp1 = fopen("t1.txt","w");                                            
	        fclose(fp1);
	                       
	        fp2 = fopen("t2.txt", "w");
	        fclose(fp2);

            fp3 = fopen("t3.txt","w");                                           
	        fclose(fp3);

            char* prog2[] = { "mkdir", "Dir1", 0};
            ret = execvp(prog2[0], prog2);    // executing ls -F 
            if(ret == -1)
            {
                perror("exec error prog 2"); 
                exit(0); 
            }

     
            V(sem1); 
            exit(0);   //==================================================== END CHILD 2 PROCESS                
		}
		else
		{
				//Now parent process waits for the child processes to finish
			pid = wait(&status);
			printf("Process(pid = %d) exited with the status %d. \n", pid, status);
				
			SEM_OFF(sem2); //********* turn off semaphore 
				
			pid = wait(&status);
			printf("Process(pid = %d) exited with the status %d. \n", pid, status);
			     
			SEM_OFF(sem1);	//********** turn off semaphore 
		}   		
	}
}
