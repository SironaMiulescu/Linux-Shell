
#include <stdlib.h> 
#include <stdio.h>  
#include <string.h> 
#include<unistd.h>

int main_dirname( int argc, char* argv[])
{
  char *buffer=(char*)malloc(500*sizeof(char*)); // we allocate space for the variable holding the name of the directory
  if(argv[1]) // if an argument exists
	{int i=1; // we go looking from the second argument since the first one is the calling of the function 
	while(argv[i]) // while we still have arguments, since we can receive more than 1
		if(!strcmp(argv[i],"/")) // if the string is just a slash
		{
			strcpy(buffer,"/"); // we place the slash into the final string
			printf( "%s \n", buffer); // we print it 
  			i++; // we go to the next argument if there is one
  		}
		else	// if the string is not just a slash 
	
		{
	
		int last=0; //the last variable holds the position of the last "countable" slash 
	  	for(int j=0;j<strlen(argv[i]);j++)// for every character in the string 
   			if(argv[i][j]=='/' && argv[i][j+1]) // we check if the slash is "countable" if is not the trailing slash, the one that has nothing after it 
   				last=j; // if it's not the last slash, we store its position
   		if(last==0) // if there is no slash in the string
   			strcpy(buffer,"."); //
   		else		
   			{strncpy(buffer,argv[i],last); // if we have slashes in the string, we copy into the buffer everything before it and put an end to the the buffer string
   			buffer[last]='\0';}
  		printf( "%s \n", buffer);
  		i++;
      		}	
      free(buffer); // we empty the space reserved for the name
      printf("this is my function\n");
   	}
   
   
  else // if we have no string given as parameter, we print the name of the current working directory and we free the space
  	{
  		if ( getcwd( buffer, 500 )== NULL )
  	    		perror( "getcwd error" );
  	 	else
  		 {
  	    		printf( "%s \n", buffer);
  	    		free(buffer);
  	 	 }
      }

}
