//
// Created by kris on 10/23/24.
//

#include "help.h"
#include <stdio.h>
#include <string.h>

void helpAuthors(char *pcs[]) {
    if ( pcs[0]==NULL ) {
        printf("authors: Prints the names and logins of the program authors.\n");
        printf(" - authors -l prints only the logins \n");
        printf(" - authors -n prints only the names\n");
    } else if ( strcmp( pcs[0], "-l") == 0) {
        printf("authors -l prints only the logins \n");
    } else if ( strcmp(pcs[0], "-n") == 0 ) {
        printf("authors -n prints only the names\n");
    } else {
        printf("Invalid argument %s\n", pcs[0]);
    }

}

void helpPID (char *pcs[]) {
    printf("pid: Prints the pid of the process executing the shell\n");
}

void helpPPID (char *pcs[]) {
    printf("ppid: Prints the pid of the shell’s parent process.\n");
}

void helpCd (char *pcs[]) {
    printf("cd [dir]: Changes the current working directory of the shell to dir (using the chdir system call). When invoked without auguments it prints the current working directory (using the getcwd system call.\n");
}

void helpDate (char *pcs[]) {
    if ( pcs[0]==NULL ) {
        printf("date [-t|-d]: Prints the current date in the format DD/MM/YYYY and the current time in the format hh:mm:ss.n\n");
        printf(" - date -d: Prints the current date in the format DD/MM/YYYY\n");
        printf(" - date -t: Prints and the current time in the format hh:mm:ss\n");
    } else if (strcmp(pcs[0], "-d") == 0) {
        printf("date -d: Prints the current date in the format DD/MM/YYYY\n");
    } else if ( strcmp(pcs[0], "-t") == 0 ) {
        printf("date -t: Prints and the current time in the format hh:mm:ss\n");
    } else {
        printf("Invalid argument %s\n", pcs[0]);
    }

}

void helpHistoric (char *pcs[]) {
    if (pcs[0]==NULL) {
        printf("historic [N|-N] Shows the historic of commands executed by this shell. In order to do this, a list to store all the commands input to the shell must be implemented\n");
        printf(" - historic: Prints all the comands that have been input with their order number\n");
        printf(" - historic N: Repeats command number N (from historic list)\n");
        printf(" - historic -N: Prints only the lastN comands\n");
    } else if (strcmp(pcs[0], "N") == 0) {
        printf("historic N: Repeats command number N (from historic list)\n");
    } else if ( strcmp(pcs[0], "-N") == 0 ) {
        printf("historic -N: Prints only the lastN comands\n");
    } else {
        printf("Invalid argument %s\n", pcs[0]);
    }

}

void helpOpen (char *pcs[]) {
    printf("open [file] mode: Opens a file and adds it to the list of shell open files\n");
    printf("Open without arguments lists the shell open files\n");
}

void helpClose (char *pcs[]) {
    printf("close [df]: Closes the df file descriptor and eliminates the corresponding item from the list\n");
}

void helpDup (char *pcs[]) {
    printf("dup [df]: Duplicates the df file descriptor (using the dup system call, creating the corresponding new entry on the file list\n");
}

void helpInfoSys (char *pcs[]) {
    printf("infosys: Prints information on the machine running the shell\n");
}

void helpHelp (char *pcs[]) {
    printf("help [cmd]: help displays a list of available commands. help cmd gives a brief help on the usage of comand cmd\n");
}

void helpQuit (char *pcs[]) {
    printf("quit: Ends the shell\n");
}

void helpExit (char *pcs[]) {
    printf("exit: Ends the shell\n");
}

void helpBye (char *pcs[]) {
    printf("bye: Ends the shell\n");
}

void helpCommands (char *pcs[]) {
    printf("authors - pid - ppid - cd - date - historic - open - close - dup - infosys - help - quit - exit - bye"
		" - makefile - makedir - listfile - cwd - listdir - reclist - revlist - erase - delrec\n");
}

void helpMakefile (char *pcs[]) {
	printf("makefile [name]: creates a file named name\n");
}

void helpMakedir (char *pcs[]) {
	printf("makedir [name]: creates a directory named name\n");
}

void helpListfile (char *pcs[]) {
	if (pcs[0]==NULL) {
		printf("listfile [-long][-link][-acc] n1 n2: lists files\n");
		printf("\t-long: long list\n");
		printf("\t-acc: acesstime\n");
    	printf("\t-link: if it's a symbolic link it shows the contained path\n");
    } else if (strcmp(pcs[0], "-long") == 0) {
    	printf("listfile -long n1 n2: long list\n");
    } else if (strcmp(pcs[0], "-acc") == 0) {
    	printf("listfile -acc n1 n2: acesstime\n");
    } else if (strcmp(pcs[0], "-link") == 0) {
    	printf("listfile -link n1 n2: if it's a symbolic link it shows the contained path\n");
    } else {
    	printf("Invalid argument %s\n", pcs[0]);
    }
}

void helpCwd (char *pcs[]) {
	printf("cwd: list actual shell directory\n");
}

void helpListdir (char *pcs[]) {
  	if (pcs[0]==NULL) {
		printf("listdir [-hid][-long][-link][-acc] n1 n2: lists directory contents\n");
		printf("\t-long: long list\n");
    	printf("\t-hid: includes hidden files\n");
		printf("\t-acc: acesstime\n");
    	printf("\t-link: if it's a symbolic link it shows the contained path\n");
    } else if (strcmp(pcs[0], "-long") == 0) {
    	printf("listdir -long n1 n2: long list\n");
    } else if (strcmp(pcs[0], "-hid") == 0) {
    	printf("listdir -hid n1 n2: includes hidden files\n");
    } else if (strcmp(pcs[0], "-acc") == 0) {
    	printf("listdir -acc n1 n2: acesstime\n");
    } else if (strcmp(pcs[0], "-link") == 0) {
    	printf("listdir -link n1 n2: if it's a symbolic link it shows the contained path\n");
    } else {
    	printf("Invalid argument %s\n", pcs[0]);
    }
}

void helpReclist (char *pcs[]) {
	if (pcs[0]==NULL) {
		printf("reclist [-hid][-long][-link][-acc] n1 n2: lists recursively directory contents (subdirectories last)\n");
		printf("\t-long: long list\n");
    	printf("\t-hid: includes hidden files\n");
		printf("\t-acc: acesstime\n");
    	printf("\t-link: if it's a symbolic link it shows the contained path\n");
    } else if (strcmp(pcs[0], "-long") == 0) {
    	printf("reclist -long n1 n2: long list\n");
    } else if (strcmp(pcs[0], "-hid") == 0) {
    	printf("reclist -hid n1 n2: includes hidden files\n");
    } else if (strcmp(pcs[0], "-acc") == 0) {
    	printf("reclist -acc n1 n2: acesstime\n");
    } else if (strcmp(pcs[0], "-link") == 0) {
    	printf("reclist -link n1 n2: if it's a symbolic link it shows the contained path\n");
    } else {
    	printf("Invalid argument %s\n", pcs[0]);
    }
}

void helpRevlist (char *pcs[]) {
	if (pcs[0]==NULL) {
		printf("revlist [-hid][-long][-link][-acc] n1 n2: lists recursively directory contents (subdirectories first)\n");
		printf("\t-long: long list\n");
    	printf("\t-hid: includes hidden files\n");
		printf("\t-acc: acesstime\n");
    	printf("\t-link: if it's a symbolic link it shows the contained path\n");
    } else if (strcmp(pcs[0], "-long") == 0) {
    	printf("revlist -long n1 n2: long list\n");
    } else if (strcmp(pcs[0], "-hid") == 0) {
    	printf("revlist -hid n1 n2: includes hidden files\n");
    } else if (strcmp(pcs[0], "-acc") == 0) {
    	printf("revlist -acc n1 n2: acesstime\n");
    } else if (strcmp(pcs[0], "-link") == 0) {
    	printf("revlist -link n1 n2: if it's a symbolic link it shows the contained path\n");
    } else {
    	printf("Invalid argument %s\n", pcs[0]);
    }
}

void helpErase (char *pcs[]) {
	printf("erase [n1 n2 ...]: Deletes empty files or directories\n");
}

void helpDelrec (char *pcs[]) {
	printf("delrec [n1 n2 ...]: Deletes non empty files or directories recursively\n");
}

struct command h[]={
    {"authors",helpAuthors},
    {"pid",helpPID},
    {"ppid",helpPPID},
    {"cd",helpCd},
    {"date",helpDate},
    {"historic",helpHistoric},
    {"open",helpOpen},
    {"close",helpClose},
    {"dup",helpDup},
    {"infosys",helpInfoSys},
    {"help",helpHelp},
    {"quit",helpQuit},
    {"exit",helpExit},
    {"bye", helpBye},
    {"makefile",helpMakefile},
    {"makedir",helpMakedir},
    {"listfile",helpListfile},
    {"cwd",helpCwd},
    {"listdir",helpListdir},
    {"recdir",helpReclist},
    {"revdir",helpRevlist},
    {"erase",helpErase},
    {"delrec",helpDelrec},
    {NULL,NULL},
};

void Cmd_help(char *pcs[]) {
    int i;
    if ( pcs[0] == NULL ) {
        helpCommands(pcs);
        return;
    }
    for (i = 0; h[i].name != NULL; i++) {
        if ( strcmp( pcs[0], h[i].name ) == 0 ){
            (*h[i].function)(pcs+1);
            return;
        }
    }
    printf("Argument %s not found\n", pcs[0]);
}