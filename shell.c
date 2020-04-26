#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>	// for semaphore sys V operations
#include <sys/sem.h>

#define BUFF 64

union semun {		// to link both semaphores
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};

void atExit();
void exitingShell(char **history, int len);
void getUserIn(char *userIn);
void executeTree();

int main(int argc, char **argv)
{
    char userIn[BUFF]; 
    int loops = 0;  
    char *lists[20];

    // while loop for the shell command 
    while(1)
    {
        if(loops == 0)
        {
            system("clear"); //clear screen 
        }
        char* userName = getenv("USER"); 
        printf("%s@shell ~> ", userName); 
        getUserIn(userIn); 
        lists[loops] = strdup(userIn); 

        if(strcmp(userIn, "exit*") == 0) 
        exitingShell(lists,loops);
        else if(strcmp(userIn, "t") == 0)
        executeTree(); 

        loops++; 
    }
    
    return 0; 
}


/* exitingShell()*
  -> prints a list of the last 4 commands of the terminal
  -> a detail list of the current directory like ls -l
  -> wait for the user to hit return to return the control to the original shell 
*/
void exitingShell(char **history, int len ){

    int status; 
    pid_t pid;
    int ret; 
    pid = fork(); 

    if(pid == 0)
    {
        //printf("1 PPID: %d, PID: %d\n", getppid(), getpid()); 
        printf("\nPress ENTER to exit.\n"); 
        
        if(len <= 4){
            printf("\nCommands used: \n"); 
            for(int i = 0; i < len; i++)
                printf("   %s \n", history[i]); 
            if(len == 0)
            printf("exit* \n"); 
        }
        else{
            printf("\nLast four commands used: \n"); 
            for(int i = (len-4); i < len; i++) 
                printf("   %s \n", history[i]); 
        }
        puts("\n"); 
           
        // code for child 1
        
    }else if(pid < 0)
    {
        perror("ERROR creating mgmt1 process fail."); 
        exit(EXIT_FAILURE); 
    }else
    {
        pid = fork(); 
            if(pid == 0)
            {
                //P(sem2); 
                //printf("2 PPID: %d, PID: %d\n", getppid(), getpid());
                //printf("\nList of files in the current directory\n");  
                char* prog1[] = { "ls", "-l", 0};
                                ret = execvp(prog1[0], prog1);    // executing ls -F 
                                if(ret == -1){
                                        perror("exec error prog 1"); 
                                        exit(0); 
                                }
                // code for child 2
                //V(sem1); 
            }else if(pid < 0)
            {
                perror("ERROR creating mgmt1 process fail."); 
                exit(EXIT_FAILURE);
            }else
            {
                
                atExit(); 
                pid = wait(&status);
				printf("Process(pid = %d) exited with the status %d. \n", pid, status);
			
				pid = wait(&status);
				printf("Process(pid = %d) exited with the status %d. \n", pid, status);
                exit(0); 
                return; 
            }
            exit(0);
    }
    exit(0);  
}

/*  atExit will exit out the project when Enter is clicked, also the ... process.
*/
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

// get commands from the shell 
void getUserIn(char *userIn)
{
    fgets(userIn, BUFF, stdin); 
    int len; 
    len = (strlen(userIn)-1); 

    if(userIn[len]=='\n')  //fgets appends \n
        userIn[len] = '\0';
}

// semaphore is on 
int SEM_ON(int sem_id, int sem_val)
{
    union semun sem_union; 
    sem_union.val = sem_val; 
    return semctl(sem_id, 0, SETVAL, sem_union); 
}

// semaphore delete 
int SEM_OFF(int sem_id)
{
    return semctl(sem_id, 0, IPC_RMID); 
}

// wait
int P(int sem_id)
{
    struct sembuf sem_buf; 
    sem_buf.sem_num = 0; 
    sem_buf.sem_op = -1; 
    sem_buf.sem_flg = SEM_UNDO; 
    return semop(sem_id, &sem_buf, 1); 
}

// signal
int V(int sem_id)
{
    struct sembuf sem_buf; 
    sem_buf.sem_num = 0; 
    sem_buf.sem_op = 1; 
    sem_buf.sem_flg = SEM_UNDO; 
    return semop(sem_id, &sem_buf, 1); 
}

/*
tree*: this new command will create a directory and call it Dir0. Then it will change the working
directory to Dir0, and create three empty text files namely; t1.txt, t2.txt,and t3.txt, and one empty
directory , called Dir1,inside it.
*/
void executeTree(){   
    int status; 
    pid_t pid;
    int ret;                        
	if ((pid = fork()) == -1) 
	{
		//fork failed!
		perror("fork");
		exit(1);
	}
	
	if (pid == 0)
    {
        //First Child Process. Dear old dad tries to do some upda

    
        // printf("2 PPID: %d, PID: %d\n", getppid(), getpid());
        
        
      
        printf("one\n");
       /*   char* prog1[] = { "mkdir", "Dir0", 0};
                                 ret = execvp(prog1[0], prog1);    // executing ls -F 
                             if(ret == -1){
                                         perror("exec error prog 1"); 
                                         exit(0); 
                                }
        
        */
        
	}                                                               
	else
	{
		//Parent Process. Fork off another child process.
		if ((pid = fork()) == -1)
		{
			//Fork failed!
			perror("fork");
			exit(1);
		}
		if (pid == 0)
		{
			// child 2
            // changing the cwd to /tmp 
            //sleep(5);
            
    
         printf("two\n"); 

           if (chdir("/home/tardis/Dev/gitLocal/Shell/Test") != 0)  
             perror("chdir() to /Dir0 failed"); 

        
          
        
 char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
        exit(0); 
    }
    
        
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
			{
                //printf("2 PPID: %d, PID: %d\n", getppid(), getpid());
				//child 3
            
                 
                printf("three\n"); 
            
                
			}
			else
			{
				//Now parent process waits for the child processes to finish
				pid = wait(&status);
				printf("Process(pid = %d) exited with the status %d. \n", pid, status);
			
				pid = wait(&status);
				printf("Process(pid = %d) exited with the status %d. \n", pid, status);
			
				pid = wait(&status);
				printf("Process(pid = %d) exited with the status %d. \n", pid, status);
               
                
			}
			exit(0);
		}
		exit(0);
	}
	exit(0);    
}
