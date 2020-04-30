/****************************
Group Project 
Inna Baryanova
Tamara Pando 

Shell function
tree* function
list* function
exit* function 
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
#include<fcntl.h>

#define BUFF 64         // buffer 

void getUserIn(char *userIn);   // gets user input for choices 
void atExit();                  // exits shell 
void exitingShell(char **history, int len);     // execute commands when exiting 
void treeCommand();                             // tree* command 
void listCommand();                             // list* command 
void changeDirectory();                         // change dircectory 
void getCurrentDirectory();                     // get current directory 
//*************************************************************
 
int main()
{
    int loops = 0;          // just to clear screen at entering the shell 
    char userIn[BUFF];      // user input 
    char *lists[20];        // array of strings to store history of commands 
	
    while(1)
    {
        //printf("Dad Pid: %d\n",getpid()); 
        if(loops == 0)
        {
            system("clear");                //clear screen in the beginning 
        }

        char* userName = getenv("USER");        // show computer user in shell 
        printf("%s@shell ~> ", userName); 
        getUserIn(userIn); 
        lists[loops] = strdup(userIn);          // populate commands array 

        if(strcmp(userIn, "exit*") == 0)        // execute chosen commands 
        {
            exitingShell(lists,loops);
            exit(0); 
        }
        else if(strcmp(userIn, "tree*") == 0)
        {
            treeCommand();   
        }
        else if(strcmp(userIn, "list*") == 0)
        {
            listCommand(); 
        }
	    loops++; 
    }		  
}

/* exitingShell()*
  -> prints a list of the last 4 commands of the terminal if 4 or more commands entered
  -> prints a list of all commands entered if less than 4
  -> a detail list of the current directory like ls -l
  -> wait for the user to hit Enter to return the control to bash window
*/
//******************* EXITINGSHELL function *************************************************************************************************
void exitingShell(char **history, int len ){

    int status; 
    pid_t pid;
    int ret; 
    pid = fork(); 

    if(pid == 0)
    {
//===============================================================CODE CHILD 1: keep track of last four commands 
        //printf("1 PPID: %d, PID: %d\n", getppid(), getpid()); 
        if(len <= 4){                           // loop for keeping count of commands if <= 4
            printf("\nCommands used: \n\n"); 
            for(int i = 0; i < len; i++)
                printf("   - %s \n", history[i]); 
            if(len == 0)
                printf("exit* \n"); 
        }
        else{
            printf("\nLast four commands used: \n\n");        // loop for printing last 4 commands
            for(int i = (len-4); i < len; i++) 
                printf("   - %s \n", history[i]); 
        }
        puts("\n");  
        exit(0); 
//=================================================================== END CHILD 1 
    }else if(pid < 0)
    {
        perror("Error creating child process\n"); 
        exit(EXIT_FAILURE); 
    }else
    {
        pid = fork(); 
        if(pid == 0)
        { 
//====================================================================== CODE CHILD 2: print ls - l to screen    
            sleep(2); 
            //printf("2 PPID: %d, PID: %d\n", getppid(), getpid()); 
            char* prog1[] = { "ls", "-l",0};
            ret = execvp(prog1[0], prog1);                          // executing ls -F 
            if(ret == -1)
            {
                perror("exec error prog 1"); 
                exit(0); 
            }
//==================================================================================== END CHILD 2
        }
        else if(pid < 0)
        {
            perror("Error creating child process\n"); 
            exit(EXIT_FAILURE);
        }
        else
        {
            pid = wait(&status);
			//printf("Process(pid = %d) exited with the status %d. \n", pid, status);  //for testing
			
			pid = wait(&status);
			//printf("Process(pid = %d) exited with the status %d. \n", pid, status);  //for testing 
        }    
    } 
    atExit();   
}


/* atExit()
    -> exit the shell when Enter is clicked
    -> will print dot exiting 
*/
//******************* ATEXIT function *************************************************************************************************
void atExit()
{
    printf("\nPress ENTER to exit\n"); 
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

/* getUserIn(char *userIn)
  -> takens in input from main and appends string ends 
*/
//******************* GETUSERIN function *************************************************************************************************
void getUserIn(char *userIn)
{
    fgets(userIn, BUFF, stdin); 
    int len; 
    len = (strlen(userIn)-1); 

    if(userIn[len]=='\n')  //fgets appends \n
        userIn[len] = '\0';
}

/* treeCommand()
    -> creates two processes 
    -> process 1: creates Dir0 in current working directory 
    -> pricess 2: changes current working directory to Dir0 and creates t1.txt, t2.txt, t3.txt and Dir1
*/
//******************* TREECOMMAND function *************************************************************************************************
void treeCommand()
{
    FILE *fp1, *fp2, *fp3;			/* File Pointers */

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
	{   //==================================================== CHILD 1 PROCESS: create Dir0 in current working directory       
        //printf("1 PPID: %d, PID: %d\n", getppid(), getpid());  //for testing
        //printf("one\n");
        printf("Creating tree in current directory...\n"); 
        char* prog1[] = { "mkdir", "Dir0", 0};
        ret = execvp(prog1[0], prog1);    // executing ls -F 
        if(ret == -1)
        {
            perror("exec error prog 1"); 
            exit(0); 
        }                              
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
		{  //==================================================== CHILD 2 PROCESS: create 3 text files and Dir1
            //printf("2 PPID: %d, PID: %d\n", getppid(), getpid());
            sleep(2); 
            //printf("two\n"); 
            changeDirectory(); 
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
            //==================================================== END CHILD 2 PROCESS                
		}
		else
		{
				//Now parent process waits for the child processes to finish
			pid = wait(&status);
			//printf("Process(pid = %d) exited with the status %d. \n", pid, status);   //for testing 
					
			pid = wait(&status);
			//printf("Process(pid = %d) exited with the status %d. \n", pid, status);   //for testing 
		}   		
	}
}

/* listCommands()
    -> creates two processes 
    -> Process 1: changes directory to Dir0, clears screen, executes ls -l commands to t1.txt via redirecting the stadout using pipes 
    -> Process 2: renames t1.txt to tree.txt and executers ls -l to shell. 
*/
void listCommand()
{
    int status; 
    pid_t pid;
    int ret; 
    int pipefds[2]; // creating pipe 
    int filefd;
    
    if(pipe(pipefds) < 0){  // error handling for pipe 

        perror("pipe error"); 
        exit(1); 
    }

	if ((pid = fork()) == -1)
	{
			//Fork failed!
		perror("fork");
		exit(1);
	}

	if (pid == 0)
	{   //==================================================== CHILD 1 PROCESS: clear screen, write ls command to t1.txt             
        //printf("1 PPID: %d, PID: %d\n", getppid(), getpid()); 
        //printf("One\n"); 
        system("clear"); 
        printf("Creating list, writing to t1.txt and renaming file to tree.txt...\n\n"); 
        changeDirectory(); 
       
        filefd = open("t1.txt", O_RDWR); 
        close(1); 
        dup(filefd); 
        char* prog1[] = { "ls", "-l",0};
        ret = execvp(prog1[0], prog1);                          // executing ls -F 

        if(ret == -1)
        {
            perror("exec error prog 1"); 
            exit(0); 
        }
    }
	//==================================================== END CHILD 1 PROCESS 
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
		{  //==================================================== CHILD 2 PROCESS: Rename t1.txt to tree.txt and print ls to terminal 
            //printf("2 PPID: %d, PID: %d\n", getppid(), getpid());
            sleep(2); 
            close(filefd);
            //printf("two\n"); 

            changeDirectory(); 
            int ret = rename("t1.txt", "tree.txt"); 
            if(ret == -1)
            {
                perror("rename error\n"); 
                exit(EXIT_FAILURE); 
            }
            
            char* prog1[] = { "ls", "-l",0};
            ret = execvp(prog1[0], prog1);                          // executing ls -F 
            if(ret == -1)
            {
                perror("exec error prog 1"); 
                exit(0); 
            }
 //==================================================== END CHILD 2 PROCESS                
		}
		else
		{ 
				//Now parent process waits for the child processes to finish
			pid = wait(&status);
			//printf("Process(pid = %d) exited with the status %d. \n", pid, status);   //for testing
				
			pid = wait(&status);
			//printf("Process(pid = %d) exited with the status %d. \n", pid, status);	  //for testing    
		}   		
	}
}

/* changeDirectory()
  -> get current directoy, append Dir0/ and change to new appeneded directory 
*/
void changeDirectory()
{
    char cwd[PATH_MAX];
     
    if (getcwd(cwd, sizeof(cwd)) != NULL) 
    {
        //printf("Current working dir: %s\n", cwd);  // for testing 
    } 
    else 
    {
        perror("getcwd() error");
        exit(0); 
    }

    strcat(cwd, "/Dir0");   // append /Dir0
        
    if (chdir(cwd) != 0)                //change directory 
        perror("chdir() to /Dir0 failed"); 

    char a[PATH_MAX];
    if (getcwd(a, sizeof(a)) != NULL) 
    {
        //printf("Current working dir: %s\n", a);   //for testing 
    } 
    else 
    {
        perror("getcwd() error");
        exit(0); 
    }
}

/* getCurrentDirectory()
  -> gets current directory 
*/
void getCurrentDirectory()
{
    char cwd[PATH_MAX]; 
    if (getcwd(cwd, sizeof(cwd)) != NULL) 
    {
        //printf("Current working dir: %s\n", cwd); //for testing 
    } 
    else 
    {
        perror("getcwd() error");
        exit(0); 
    }
}
