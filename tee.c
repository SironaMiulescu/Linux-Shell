
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<string.h>


int main_tee(int argc,char *argv[] )
{ FILE *file; // file for writing into 
  int aflag=0,c;  
   size_t len = 0;
  char *line = NULL;
	while ((c = getopt (argc, argv, "a")) != -1) // get opt get the options one by one
		switch(c)
			{

				case 'a': // if we receive an a we set the flag
					aflag=1;
					
					break;
				case '?': // otherwise print a message
					printf("This option is not known\n");
					break;
			}
	
  
		
  	
  	while(getline(&line, &len, stdin)>1 ) // while the line is not NULL or new line
  		{
  			printf("%s", line); // we print it to stdout
  			for(int i=optind;i<argc;i++) // we loop through the given files, if there are multiple 
  				{	
  					if(aflag) //if we have to append
  						{ // open in append mode
  							file=fopen(argv[i],"a");
  						}
  					else
  						{ if(i==0) // for the first loopr we open for writing and then for appending to not overwrite
  							file=fopen(argv[i],"w");
  						  else
  							file=fopen(argv[i],"a");
  						}
  					// we put the line and close the file
  					fputs(line,file);
  					fclose(file);
  				
  				
  				}
  			
  	
  			
  			
  			
  			
  			
  			
  		}
  		
  free(line);
 printf("this is my function\n");
  
  
  
  return 0;

}

