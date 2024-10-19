#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <fcntl.h>

#include "historicList.h"
#include "fileList.h"

#define MAX 2048

hList commandHistory;
fList openFiles;


int breakLine(char *line, char *pcs[]) {
    if ( ( pcs[0] = strtok(line, " \t\n") ) == NULL ) {
        return 0;
    }
    int i = 1;
    while ( ( pcs[i] = strtok(NULL, " \t\n") ) != NULL ) {
        i++;
    }
    return i;
}

void doCommand(char *pcs[]);

struct command{
    char *name;
    void (*function) (char**);
};

void printAuthors(char *pcs[]){
    if ( pcs[0] == NULL ) {
        printf("Kris Santiso Sampedro\tkris.santiso@udc.es\nCris\tcorreo.cris@udc.es\n");
    } else if ( strcmp( pcs[0], "-l" ) == 0 ) {
        printf("kris.santiso@udc.es\ncorreo.cris@udc.es\n");
    } else if ( strcmp( pcs[0], "-n" ) == 0) {
        printf("Kris Santiso Sampedro\nCris\n");
    } else {
        printf("Invalid argument\n");
    }
}

void printFiles(fList fileList){
	if ( fIsEmptyList(fileList) == true ) {
    	printf("No open files\n");
        return;
    }
    fPos p = fFirst(fileList);
    while ( p != NULL ) {
        fItem item = fGetItem(p, fileList);
        if ( item.fileMode == 0 ) {
        	printf(" - %d > %s\n", item.fileDescriptor, item.fileName);
        } else {
        	printf(" - %d > %s - %d\n", item.fileDescriptor, item.fileName, item.fileMode);
        }

        p = fNext(p, fileList);
    }
}

void openFile(char *pcs[], fItem item){
    if ( pcs[0] == NULL ){
        printf("No argument introduced\n");
        return;
    }
    int i, df, mode = 0;
    for (i=1; pcs[i]!=NULL; i++){
      	if (!strcmp(pcs[i],"cr")) mode|=O_CREAT;
      	else if (!strcmp(pcs[i],"ex")) mode|=O_EXCL;
      	else if (!strcmp(pcs[i],"ro")) mode|=O_RDONLY;
      	else if (!strcmp(pcs[i],"wo")) mode|=O_WRONLY;
      	else if (!strcmp(pcs[i],"rw")) mode|=O_RDWR;
      	else if (!strcmp(pcs[i],"ap")) mode|=O_APPEND;
      	else if (!strcmp(pcs[i],"tr")) mode|=O_TRUNC;
      	else break;
    }
    if ((df=open(pcs[0],mode,0777))==-1) {
        perror ("Imposible abrir fichero");
    } else {
        item.fileDescriptor = df;

        if ( mode == 0 ) {
        	item.fileMode = 0;
        } else {
        	item.fileMode = fcntl(df, F_GETFL);
        }
        fInsertItem(item, &openFiles);

        printf ("Anadida entrada %d a la tabla ficheros abiertos \n", item.fileDescriptor);
    }
}


void printFile(char *pcs[]){
  	if ( pcs[0] == NULL ) {
        printFiles(openFiles);
        return;
  	}
    fItem item;
    item.fileName = strdup (pcs[0]);

    openFile(pcs, item);


}

void closeFile(char *pcs[]){
  	int df;
	if (pcs[0]==NULL || (df=atoi(pcs[0]))<0) {
          printf("Argument invalid or inexistent\n");
          return;
    }
    if (close(df)==-1) {
    	perror("Inposible cerrar descriptor");
        return;
    }
    fPos p = fFirst(openFiles);
    while ( p != NULL ) {
        fItem item = fGetItem(p, openFiles);
    	if (item.fileDescriptor == df) {
            fDeleteAtPosition(p, &openFiles);
            return;
    	}
        p = fNext(p, openFiles);
    }
}

void dupFile(){

}

void printHistoric(hList history) {
    if ( hIsEmptyList(history) ) {
        printf("No commands in history\n");
    } else {
        hPosL p = hFirst(history);
        while ( hNext(p,history) != NULL ) {
            hItemL item = hGetItem(p, history);
            printf("- %d > %s",item.commandNumber, item.command);
            p = hNext(p, history);
        }
    }

}

void printNHistoric(int num, hList history) {

    int i = 0;
    num = -num;


    hPosL p = hLast(history);

    while ( i != num ) {
        hItemL item = hGetItem(hPrevious(p, history), history);
        printf("- %d > %s",item.commandNumber, item.command);
        p = hPrevious(p, history);
        i++;
    }
}

void execHistoric (int num, hList history) {
    char *pcs[MAX/2];
    hPosL p = hFirst(history);

    for (int i = 1; i < num; i++) {
        p = hNext(p, history);
    }
    hItemL item = hGetItem(p, history);

    breakLine(item.command, pcs);

    doCommand(pcs);
}

void historic(char *pcs[]) {

    if (pcs[0]==NULL) {
        printHistoric(commandHistory);
        return;
    }
    char *end;
    long int num;
    num = strtol(pcs[0], &end, 10);

    if (*end != '\0') {
        printf("Invalid argument %s\n", pcs[0]);
        return;
    }
    if ( *pcs[0]==*"-" ) {
        printNHistoric(num, commandHistory);
    } else {
        execHistoric(num, commandHistory);
    }
}

void getPID(char *pcs[]) {
    pid_t process_id = getpid();
    printf("Process ID: %d\n", process_id);
}

void getPPID (char *pcs[]) {
    pid_t parent_id = getppid();
    printf("Parent process ID: %d\n", parent_id);
}

struct utsname systemUname;
void systemInfo (char *pcs[]) {
    if( uname(&systemUname) < 0 ) {
        perror("uname");
    } else {
        printf("System name: %s\nNodename: %s\nRelease: %s\nVersion: %s\nMachine: %s\n", systemUname.sysname, systemUname.nodename, systemUname.release, systemUname.version, systemUname.machine);
    }
}

void getDate (char *pcs[]) {
    time_t t;
    time(&t);

    struct tm * time;
    time = localtime(&t);

    if (pcs[0]==NULL) {
        printf("%.2d %.2d %.2d\n%.2d:%.2d:%.2d\n", time->tm_mday, time->tm_mon, time->tm_year + 1900, time->tm_hour, time->tm_min, time->tm_sec);
    } else if ( strcmp( pcs[0], "-d") == 0 ) {
        printf("%.2d %.2d %.4d\n", time->tm_mday, time->tm_mon, time->tm_year + 1900);
    } else if ( strcmp( pcs[0], "-t") == 0 ) {
        printf("%.2d:%.2d:%.2d\n", time->tm_hour, time->tm_min, time->tm_sec);
    } else {
        printf("Invalid argument %s\n", pcs[0]);
    }
}

void changeDir (char *pcs[]) {
    if ( pcs[0]==NULL ) {
        char dir[MAX];
        if (getcwd(dir, sizeof(dir)) != NULL) {
            printf("%s\n", dir);
        } else {
            perror("getcwd() error");
        }
    } else {
        if ( chdir(pcs[0]) != 0 ) {
            perror("chdir() error");
        }
    }
}

void exitShell (char *pcs[]) {
  	if ( !fIsEmptyList(openFiles) ) {
		fDeleteAll(&openFiles);
    }
    if ( !hIsEmptyList(commandHistory) ) {
		hDeleteAll(&commandHistory);
    }
    exit(0);
}

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
    printf("authors - pid - ppid - cd [dir] - date [-d/-t] - historic [N/-N] - open [file] mode - close [df] - dup [df] - infosys - help [cmd] - quit - exit - bye\n");
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
    {NULL,NULL},
};

void generalHelp(char *pcs[]) {
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

struct command c[]={
    {"authors", printAuthors},
    {"help", generalHelp},
    {"historic",historic},
    {"pid",getPID},
    {"ppid",getPPID},
    {"infosys",systemInfo},
    {"exit",exitShell},
    {"quit",exitShell},
    {"bye",exitShell},
    {"date", getDate},
    {"cd",changeDir},
    {"open",printFile},
    {"close",closeFile},
    {"dup",dupFile},
    {NULL, NULL},
};

void doCommand(char *pcs[]) {
    int i;
    if ( pcs[0] == NULL ) {
        return;
    }
    for (i = 0; c[i].name != NULL; i++) {
        if ( strcmp( pcs[0], c[i].name ) == 0 ){
            (*c[i].function)(pcs+1);
            return;
        }
    }
    printf("Command %s not found\n", pcs[0]);
}

/*
int readCommand(char *pcs[], tList *history, int commandNumber) {
    char line[MAX];
    char command[MAX];

    printf("-> ");
    fgets(line, MAX, stdin);

    strcpy( command, line);

    if (breakLine(line, pcs) == 0) {
        return 1;
    }

    tItemL item;
    item.commandNumber = commandNumber;
    strcpy( item.command, command);

    insertItem(item, NULL, history);

    return 0;
}*/



/*
int main(){
    int commandNumber = 1;
    char *pcs[MAX/2];

    tList history;
    createEmptyList(&history);

    while (true) {
        if ( readCommand(pcs, &history, commandNumber) == 1) {
            continue;
        }
        doCommand(pcs);
        commandNumber++;
    }

    return 0;
}*/

void addOpenFiles(hList *commandHistory){
    fItem item;
    int i;
    for ( i = 0; fcntl(i, F_GETFL) != -1; i++ ) {
		item.fileDescriptor = i;
		item.fileName = "stdi";
		item.fileMode = fcntl(i, F_GETFL);
		fInsertItem(item, &openFiles);
    }

}

int main(){
    char line[MAX];
    char *pcs[MAX/2];
    int commandNumber = 1;
    char command[MAX];

    hCreateEmptyList(&commandHistory);
    fCreateEmptyList(&openFiles);

	addOpenFiles(&commandHistory);

    while (1) {

        printf("-> ");
        fgets(line, MAX, stdin);

        strcpy(command, line);
        if (breakLine(line, pcs) == 0) {
            continue;
        }

        hItemL item;
        item.commandNumber = commandNumber;
        item.command=command;
        hInsertItem(item, &commandHistory);

        commandNumber++;

        doCommand(pcs);
    }

    return 0;
}