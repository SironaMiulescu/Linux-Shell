#include<stdio.h>
#include <readline/readline.h>
 #include <readline/history.h>
#include<stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include"dirname.h"
#include"cp.h"
#include"tee.h"
char *result[200]; //this is the global variable which holds the commands  after pipe tokenizing
char *command[200]; //this is the global variable which holds the commands  after space tokenizing
char *output_redir; // path to the output redirecting file
int output_redirection; // variable used to indicate redirection


void help();
void version();
void handle(int argc); 
void tokenize_on_spaces(char *s) ;
void tokenize_on_pipes(char *s) ;
int call_extern (int input, int first, int last, char *command[]);
char *skip_multiple_whitespaces(char *s);
char *tokenize_redirect_output (char *cmd_exec);



void help()
{
printf("The commands implemente for the second part are:\n");
printf("DIRNAME:\n\t no options implemented.\n\t This function is supposed to print the name of the directory of the path you specify. It reveives as parameters a string\n");
printf("TEE:\n\t [-a] When this option is chosen, we append to the file istead of overwriting.\n\t This function is supposed to add or write in a file the string received from stdin and also print it to stdout\n"); 
printf("CP:\n\t [-i] When this option is chosen, we are asked if we want to overwrite a file if it's not empty.\n\t [-v] When this option is chosen, we are shown the source and destination file.\n\t [-r]/[-R] If this option is chosen ,it means we have to recursively copy the structure of a folder into a source folder, or if the source folder already exists, we create a folder inside it with the structure of source folder.\n\t [-t] This option is getting the target directory,and all the other files are treated as regular files \n\tThis function is supposed to copy file from source file to destination file\n");


}

void version()
{

printf("This project was created by Sirona Miulescu\n");
printf("Global variable and their purpose:\n\t char *result[size]- used for holding the command after tokenizing by pipe\n\t char *command[size]-used for holding one command and its options and arguments\n\t char *output_redir-holds the path of the fie into which we redirect, it is obrained in tokenize_redirect_output()\n\t int output_redirection- this integer decides whether we enter a part of the program in call_extern ans is set to 1 if the command has > present\n");
printf("Functions and their purpose:\n\t handle(int argc)-prepares all commands for execution, this means tokenizing,spliting on spaces and setting the input,deciding wheter command is first or last or if it needs to be split on >\n\t tokenize_on_space(char *s)-self explanatory\n\ttokenize_on_pipes(char*s)-self explanatory\n\t call_extern (int input, int first, int last, char *command[])- an executes the commands, sets the corresponding files for input and output and returns the filedescriptor of the file that reads from the pipe which will we used instead of stdin\n\t skip_multiple_spaces(char *s)\n\t tokenize_redirect_output(char *command to execute)- saves the file into which wewrite into a global variable and returns the command that needs to be executed\n");
}



// the string received as parameter is split on '>'. the first part, the command in itself, is placed into val[0] and returned and the function is saved into the global variable command
char *tokenize_redirect_output (char *cmd_exec) 
	{
		output_redirection=1; // we set the value to one to indicate redirection
		char *val[3];



		val[0] = strtok(cmd_exec, ">");// the first part, before the >
		val[1] = strtok(NULL,">"); // the part after the >, not including it
		val[2]=NULL;
		output_redir=strdup(val[1]); // put it in the global variable
		return val[0];
	
	}

// this function takes as argument a string and splits it based on a single whitespace. It places the command, its arguments and options accordingly in the next available space in the global variable
void tokenize_on_spaces(char *s) 
	{

		int count = 1; //start from 1 since position 1 is occupied
		command[0] = strtok(s, " "); //placing the first argument


		while((command[count] = strtok(NULL, " ")) != NULL)  //while we still have spaces to split on 

			{
   
    
				    count++;
    
			}
		command[count]=NULL; 


	}

// preprocessing the command, skiping multiple spaces, tokenizing and calling the executing function
void handle(int argc)
		{
			int input=0; //this variable is the file descriptor for the middle functions
			char *new=NULL; //temporary string to hold the command through preprocessing

			for(int i=0;i<argc-1;i++) //all commands except the last one
				{
					if(strchr(result[i],'>')) // if they have redirection
						{
							new=tokenize_redirect_output(result[argc-1]);// capture the redirecting file and get the commands

							new=skip_multiple_whitespaces(new); 
							tokenize_on_spaces(new); // identify the command and its options
				
						}
					else
						{	new=skip_multiple_whitespaces(result[i]);
							tokenize_on_spaces(new);}
					
					if(i==0)
						{
							input=call_extern(input,1,0,command); // if the function is the first one, we call the executing funtion specifying it is the first and we read 														from stdin (input=0=stdin, first=1,last=0, the command) 
							
						}
					else
						{
							input=call_extern(input,0,0,command); // if the function is some middle one, it is neither first or last and the input in the file descriptor 														received after execution of call_extern( first=0,last=0, input=read end of pipe) 

							
						}
				}
							
										
			if(strchr(result[argc-1],'>')) // we do the same checkings as before, but in all cases, in place of last we have 1
				{
					new=tokenize_redirect_output(result[argc-1]);
					new=skip_multiple_whitespaces(new);
					tokenize_on_spaces(new);
					
				
				}
				
			else
				{	new=skip_multiple_whitespaces(result[argc-1]);
					tokenize_on_spaces(new);
				}			
			input=call_extern(input,0,1,command);// ( input=read end from the pipe of the process executed before (mypipefd[0]), first=0,last=1)

	}
// places the commands, split only by pipe in the next free slot in the global variable result
void tokenize_on_pipes(char *s) 
{

	int count = 1;
	result[0] = strtok(s, "|");


	while((result[count] = strtok(NULL, "|")) != NULL) 
		{
   			 count++;
    
		}
	result[count]=NULL;

	handle(count);
}


/*
	This function is only called if the command you are trying to excute is not already in-built(echo,help,history,exit)
	It forks, for the command entered to execute on its own, not to interrupt the shell,while the shell waits for the child to finish
	This function will be modified when pipes are added
*/
int call_extern (int input, int first, int last, char *command[]) 
{

	int mypipefd[2], p,pid,output;
	//in this order we have:  the array of file descriptors(mypipef[0]-read end, mypipefd[1]-write end), the return value for pipe, the process id (0 -child,<0 error,>0 parent), output 		redirection file descriptor
	// we create the pipe and check if it was successful
	p = pipe(mypipefd);
	if (p==-1) {
		perror("pipe error: ");
		return 1;
	}
	//creating a child process
	pid = fork();
	if(pid<0)
		perror("the forking was unsuccsessful");
	else
	{
	if (pid == 0) // if we are in the child process
	 {

		if (first && !last && !input) // first=1,last=0,input=0 means we have the first command
		{
			// the first command only writes into the pipes since it takes the arguments from stding so we only need to place our write end of the pipe in place of the stdout
			dup2 (mypipefd[1], 1);
			
		}
		else if (!first  && !last && input ) {
			// first=0,last=0,input!=0 means we are in some middle command so this means we have to replace both input and output files(0 -in,1-out)
			dup2 (input, 0); //placing the read end of the previous pipe as input
			dup2 (mypipefd[1], 1); // placing the write end in place of stdout
			}
		else {
			// we are at the last one so we onlu need to read from pipe so we place the read end of previous pipe to our input 
			dup2 (input, 0);
			
		}
		if (output_redirection==1) {
			// if the global variable was set in the redirection tokenizing function we enter the function 
			// we open the file it specified after the > which we saved  and get its file descriptor
			output = fileno(fopen(output_redir,"w"));
			
			if (output < 0) {

				fprintf(stderr, "Failed to open %s for writing\n", output_redir);
				return (EXIT_FAILURE);
			}
			// we place int the place of standard output the file descriptor of the file specified and close it afterwards
			dup2 (output, 1);
			
			
		
		}
	if(!strcmp(command[0],"help"))
		help();
		else
			if(!strcmp(command[0],"version"))
				version();
			else	
				if(!strcmp(command[0],"dirname"))
					{
					int i=0;
					while(command[i])
						i++;
					
					main_dirname(i,command);}
				else
					if(!strcmp(command[0],"cp"))
					{
					int i=0;
					while(command[i])
						i++;
					
					main_cp(i,command);}
					
					else
						if(!strcmp(command[0],"tee"))
							{
							int i=0;
							while(command[i])
								i++;
							
							main_tee(i,command);
							}
					
						else
					
							 if (execvp(command[0],command) < 0) 
							 	{
									fprintf(stderr, "%s: Offending command name\n",command[0]);
								}
		exit(0);
	}

	
	else{
	// wait for the child process to finish  
	waitpid(pid,0,0);}
	// here we close all files we used, except the read end of pipe so that it can be used as input for the next command
	if(last)
	
		close(mypipefd[0]);
	if(input!=0)
		close(input);
	close(mypipefd[1]);	
	if(output_redirection==1)
		{
			
			close(output);
			output_redirection=0;
		}
	return (mypipefd[0]);
	}
}





//This function skips multiple spaces in the command

char* skip_multiple_whitespaces(char *s) {
    char *temp=(char*) malloc(sizeof(char*)*strlen(s)+1); // we create a temporary char pointer which holds the final value
    int i=0,j=0; // the indexes fo both arrays
    while(s[i]) //while we haven't reached the end of the string
    { 
    	if(s[i]!=' ') // if the value is different from space, it doesn't bother us, so we place it in the final string
    	{temp[j]=s[i];
    	i++;
    	j++;	//we increment both idexes, preparing for looking at the next position 
    	}	
    else
    	{if(s[i]==' ' && s[i+1]==' ') // if this situation is ecountered, meaning we have multiple spaces, we onnly go further, without placing them in the final string 
    		i++;
    	if(s[i]==' ' && s[i+1]!=' ') // if we find a space, but it is followed by a character we place it into the final string since we need a space based on which we split  
    		{temp[j]=s[i];
    		i++;
    		j++;	
    		}
    	}
    }
    temp[j]='\0'; // we put an end to the final string.
   return temp;
}

// this function splits the string received as parameter on whitespace



int main()
{
    
			
system("clear"); // we clear the console
while(1){
	char *command=readline("MyShell:"); // this function does quite a lot. It handles arrows being pressed, it reads the command and it also prints the custom message
	if(!command) // if the command somehow has't been allocated right
		exit(0);// we exit
	
	if(!strcmp(command,"exit")) // if the command is the exit one 
			{
			printf("We are closing the shell. Thank you!\n"); // print a goodbye message and exit 
			 
			exit(0);
			}

	else 
		if (!(strcmp(command, "\n") && strcmp(command,"")))
		continue;
				else
			// if it is any other command, we add it to the history and send it for handling
			// I will add more cases, like empty command line or command on multiple lines
	
				{
					add_history(command); 
			
					tokenize_on_pipes(command);
		
		
			
				}
	
		
	free(command); // we free the space taken by the char pointer;
	 }
	return 0;


}


