# Linux-Shell
This repository contains a basic linux shell with three other commands, namely cp,tee and dirname.

The shell runs by entering  "gcc -o myShell myShell.c -lreadline tee.c cp.c dirname.c" in the terminal 

The shell offers the following functionalities:\
  -searching through past commands by using up and down arrow keys;\
  -exiting on entering the 'exit' command\
  -it offers some help regarding the usage of the  three commands. This is done by entering the 'help' command\
  -by entering 'version' in the terminal, information about the author and general information about the shell will be printed\
  -also supporting multiple pipes, input & output redirection \
  -a farewell message is printed at the closing of the shell\
  
 The three commands behave like the three linux commands. Minor adjustments have been made at dirname to print the current directory if no string was given 
 
 The shell and the commands were implemented for the Operating Systems assignment, lecture held at West University of Timişoara.
