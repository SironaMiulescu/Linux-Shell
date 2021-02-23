#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<libgen.h>

void copyFILE(char *path_s, char* path_d, int iflag, int vflag)
{
	FILE *source,*destination;
	char c;
	DIR *dir=opendir(path_d); // if the drectory opens, it means we do no have a file as destination
	if(dir)
		{fprintf(stderr,"%s:This is a directory,not a file \n",path_d);
		closedir(dir);}
	else{
	if(iflag) // if we have the info flag on, we need to ask if we want to overwrite be it that the file is not empty
		{

			destination = fopen(path_d, "r"); // we try opening the file in readmode
			if(destination==NULL) // if the file does not open, we create one by opening the path in write mode
				{
					// we open both files, read from source(path_s) and write into destination(path_d) and when we have reached the end, we close both files
					source=fopen(path_s,"r");
					destination=fopen(path_d,"w");
        				c = fgetc(source);
    					while (c != EOF)
    						{
        						fputc(c, destination);
      							c = fgetc(source);
    						}
    					if(vflag) // if verbose flag is on, we show the path of the input file and the path of the source file
    						printf("%s -> %s\n", path_s, path_d);
					fclose(source);
    					fclose(destination);


				}
			else
				{
			// if we can open the destination file, it might not be empty sa we open it in readmode, we seek to the end and if the size is 0, the file is empty and we copy normaly
					fseek(destination, 0, SEEK_END);
					int size = ftell(destination);
					fclose(destination);
					if(!size)
						{
							source=fopen(path_s,"r");
							destination=fopen(path_d,"w");
        						c = fgetc(source);
    							while (c != EOF)
    								{
        								fputc(c, destination);
      									c = fgetc(source);
    								}
    							if(vflag)
    								printf("%s -> %s\n", path_s, path_d);
							fclose(source);
    							fclose(destination);

						}
					else // if the size is not 0, the file is not empty so we need to ask before we overwrtie
						{
							char response;
							printf("Do you want to overwrite %s? y/n \n",path_d);
							scanf("%c",&response);
							if('y'==response) // if we answer yes, we simply open destination in write mode and overwrite, otherwise, we skip this step alltogether
								{
									source=fopen(path_s,"r");
									destination=fopen(path_d,"w");
        								c = fgetc(source);
    									while (c != EOF)
    										{
        										fputc(c, destination);
      											c = fgetc(source);
    										}
    									if(vflag)
    										printf("%s -> %s\n", path_s, path_d);
									fclose(source);
    									fclose(destination);

						 		}
						}

			}
		}
	else // if we don't have the info flag on we simply overwrite if it's the case and show tha paths if verbose is set
		{
			source=fopen( path_s,"r");
			destination=fopen(path_d,"w");
        		c = fgetc(source);
    			while (c != EOF)
    				{
        				fputc(c, destination);
      					c = fgetc(source);
    				}
    			if(vflag)
    				printf("%s -> %s\n", path_s,path_d);
			fclose(source);
    			fclose(destination);

		}}



}






int copyRecursively(char *path_source, char *path_destination,int iflag, int vflag)
	{
		char *path_s=(char*)malloc(200*sizeof(char*));// variables for holding the path the deeper we get into the directory
		char *path_d=(char*)malloc(200*sizeof(char*));

		int info=iflag,verbose=vflag; // info and verbose flags to send to copyFILE
		struct dirent *dp;
		DIR *dir=opendir(path_source);

		FILE *source,*destination;
			//if the directory does not open it means we have a file and we copy the source file to the destination, the paths being already created


		 	if(!dir)

				{
					copyFILE(path_source,path_destination,info,verbose);
					return 0 ;
					//file is the base case so when we reach a file we exit the function

				}
			else	 // if the directory opens, we need to make a new directory in the source so we keep the structure
				mkdir(path_destination,0777);



		while((dp=readdir(dir))!=NULL) // while we can read the directory
			{

			if(strcmp(dp->d_name,".")!=0 && strcmp(dp->d_name,"..")!=0)	// if the name of the file is not some upper redirection or the current working directory, we create the new paths
											//  by placing / after the string which takes us into the directories and we continue reading from there
				{
				strcpy(path_s,path_source);
				strcat(path_s,"/");
				strcat(path_s,dp->d_name);
				strcpy(path_d,path_destination);
				strcat(path_d,"/");
				strcat(path_d,dp->d_name);

				copyRecursively(path_s,path_d,info,verbose);
				}
			}
		closedir(dir);
		free(path_s);
		free(path_d);
}

int main_cp(int argc, char *argv[])
{	FILE *source,*destination;
	int c;
	int iflag = 0,rflag=0,tflag=0,vflag=0,count=0;
	char *tvalue=NULL;
	while ((c = getopt (argc, argv, "irRvt:")) != -1) // for each option we get, wet the corresponding flag and if it is the case, we increment the count of arguments. If the option is t we save 									the arguments received in tvalue
		switch(c)
		{
			case 'i':
				iflag=1;
				count++;
				break;

			case 'r':

				rflag=1;

				break;

			case 'R':

				rflag=1;

				break;

			case 't':
				tflag=1;
				tvalue=optarg;
				count++;
				break;

			case 'v':
				count++;
				vflag=1;
				break;

			case '?':
				printf("This option is not known\n");
				break;
		}
	// if the recursive flag is set
	if(rflag)
		{ // if we specified a target directory
			if(tflag)
				{ // we try to open the directory specified after the option t:
				DIR *dir = opendir(tvalue);
				if(dir)
					{ // if the directory opens, it means we have to create another one inside it
						char *new_directory=(char*)malloc(256*sizeof(char));
							// we recreate the new path of the files( old_folder/new_folder) and create the new folder with all the permissions
						strcpy(new_directory,tvalue);
						strcat(new_directory,"/");
						strcat(new_directory,argv[optind]);
						mkdir(new_directory,0777);
						 // we call the recusive copying with the path of the source directory, placed in the position of the first file to be moved(optind), and source 								folder and the info and verbose flags
						copyRecursively(argv[optind],new_directory,iflag,vflag);
						free(new_directory);


					}
				else
					{
						// if the directory is not opening, we create a new one and call the same function as before
						mkdir(tvalue,0777);
						copyRecursively(argv[optind],tvalue,iflag,vflag);

					}

				closedir(dir);


				}
			else
			{ // if there is no target directory specified
				DIR *dir = opendir(argv[argc-1]); // we open the last argument as directory target
				if(dir) // if it opend, we create a new directory inside and make a new path, as before
					{
						char *new_directory=(char*)malloc(256*sizeof(char));

						strcpy(new_directory,argv[argc-1]);
						strcat(new_directory,"/");
						strcat(new_directory,argv[argc-2]);
						mkdir(new_directory,0777);
						copyRecursively(argv[argc-2],new_directory,iflag,vflag);
						free(new_directory);


					}
				else
					{
						// if it does not open ,we make a new one and call the function
						mkdir(argv[argc-1],0777);
						copyRecursively(argv[argc-2],argv[argc-1],iflag,vflag);

					}

				closedir(dir);
			}
		}
	else
		{
		// if the recursive flag is not set it means we have to deal with one or more files and a source directory or a file destination


		int counter=argc-count-1; // we check how many arguments besides the options and command we have to see which case this fits

		if(counter==2) //if we have only 2 argumnets beside the options it means we only have a source file and a possible folder of file for destination
			if(tflag) // we have a folder as destination
				{	// we open the folder if possible and make the new path as dest/source.file
					DIR *dir=opendir(tvalue);
					char *path=(char*)malloc(256*sizeof(char*));
					if(dir)
						{






									strcpy(path,tvalue);
									strcat(path,"/");
									strcat(path,argv[optind]);
									copyFILE(argv[optind],path,iflag,vflag);





						}
					else
						{// if the folder does not open , we create a new one and the path

						mkdir(tvalue,0777);





									strcpy(path,tvalue);
									strcat(path,"/");
									strcat(path,argv[optind]);
									copyFILE(argv[optind],path,iflag,vflag);


						}
					free(path);
					closedir(dir);
				}
			else
				{ // it means we have a file as destination and we only call the function

					copyFILE(argv[argc-2],argv[argc-1],iflag,vflag);


				}
		else
			if(counter>2) // if counter is bigger than 2, we have more than one source file
				if(tflag) // if we have any target directory, we open that one, otherwise we open the argument on last position

				{
					DIR *dir=opendir(tvalue);
					char *path=(char*)malloc(256*sizeof(char*));
					if(dir)
						{
						//for all files that need to be moved, we create a new path , the destination path . If the directory opens, we just copy, otherwise we create a new one
						for(int i=optind;i<argc;i++)

								{


									strcpy(path,tvalue);
									strcat(path,"/");
									strcat(path,argv[i]);
									copyFILE(argv[i],path,iflag,vflag);


								}


						}
					else
						{

						mkdir(tvalue,0777);
						for(int i=optind;i<argc;i++)

								{


									strcpy(path,tvalue);
									strcat(path,"/");
									strcat(path,argv[i]);
									copyFILE(argv[i],path,iflag,vflag);


								}

						}
					free(path);
					closedir(dir);
				}
				else // if we have no target directory specified, we open the last argument as a directory, create one if it does not exist and make the destination path
					{


					DIR *dir=opendir(argv[argc-1]);
					char *path=(char*)malloc(256*sizeof(char*));
						if(dir)
							{
								for( int i=optind;i<argc-1;i++)

										{
											strcpy(path,argv[argc-1]);
											strcat(path,"/");
											strcat(path,argv[i]);
											copyFILE(argv[i],path,iflag,vflag);
										}

							}
						else
							{
								mkdir(argv[argc-1],0777);
								for( int i=optind;i<argc-1;i++)

										{
											strcpy(path,argv[argc-1]);
											strcat(path,"/");
											strcat(path,argv[i]);
											copyFILE(argv[i],path,iflag,vflag);
										}


							}
					free(path);

					closedir(dir);

					}

		}
		printf("this is my function\n");
}
