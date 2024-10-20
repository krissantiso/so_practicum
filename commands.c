//
// Created by kris on 10/20/24.
//

#include "commands.h"
#include "historicList.h"
#include "fileList.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/utsname.h>

void Cmd_authors (char *pcs[]){
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

void Cmd_pid (){
    printf("Process ID: %d\n", getpid());
}

void Cmd_ppid (){
    printf("Parent process ID: %d\n", getppid());
}

void Cmd_cd (char *pcs[]) {
    if ( pcs[0]==NULL ) {
        char dir[MAX];
        if (getcwd(dir, sizeof(dir)) != NULL) {
            printf("%s\n", dir);
        } else {
            printf("Cannot do. Error number is %d (%s)\n", errno, strerror(errno));
        }
    } else {
        if ( chdir(pcs[0]) != 0 ) {
            printf("Cannot do. Error number is %d (%s)\n", errno, strerror(errno));
        }
    }
}

void Cmd_date (char *pcs[]) {
    time_t t;
    time(&t);

    struct tm * time = localtime(&t);

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

void execHistoric (int num, char *pcs[]) {
    if (num == 0) {
        printf("Please insert a valid number\n");
        return;
    }
    char *line = hGetItem(num, hisList);
    if(line==NULL){
        printf("There is no command at this position\n");
        return;
    }
    if (breakLine(line,pcs)>0) {
        if(DoCommand(pcs)) return;
    }
}

//me gusto mas mi funcion asi q la use y ya :p
void Cmd_historic(char *pcs[]) {

    if (pcs[0]==NULL) {
        hPrintList(hisList);
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
        hPrintNElems(num, hisList);
    } else {
        execHistoric(num, pcs);
    }
}

void Cmd_open (char *pcs[]){
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
        perror ("Impossible to open file");
    } else {
        fItemL item;
        item.fileDescriptor = df;
        item.fileName = strdup(pcs[0]);

        switch (mode) {
            case 0: strcpy(item.fileMode, "O_RDONLY"); break;
            case 1: strcpy(item.fileMode, "O_WRONLY"); break;
            case 2: strcpy(item.fileMode, "O_RDWR"); break;
            case 64: strcpy(item.fileMode, "O_CREAT"); break;
            case 128: strcpy(item.fileMode, "O_EXECL"); break;
            case 512: strcpy(item.fileMode, "O_TRUNC"); break;
            case 1024: strcpy(item.fileMode, "O_APPEND"); break;
        }

        if ( fInsertItem(item, &ofList) ) {
            printf("Added entry %d to the open files table \n", item.fileDescriptor);
        } else {
            printf("There has been an error trying to add the file\n");
        }

    }
}

void Cmd_close(char *pcs[]){
    int df;
    if (pcs[0]==NULL || (df=atoi(pcs[0]))<0) {
        printf("Argument invalid or inexistent\n");
        return;
    }
    if (close(df)==-1) {
        perror("Inposible cerrar descriptor");
        return;
    }
    if(fIsEmptyList(ofList)) {
        printf("There are no open files\n");
        return;
    }
    fPosL p = fFirst(ofList);
    while ( p != NULL ) {
        fItemL item = fGetItem(p, ofList);
        if (item.fileDescriptor == df) {
            fDeleteAtPosition(p, &ofList);
            return;
        }
        p = fNext(p, ofList);
    }
}

//Function not checked
void Cmd_dup (char * tr[])
{
    int df, duplicado;
    char aux[MAX],*p;
    fPosL pos;
    fItemL item1; //auxiliar to find the right file
    if (tr[0]==NULL || (df=atoi(tr[0]))<0) { //there is no parameter or the descriptor is less than 0
        fPrintList(ofList);
        return;
    }

    duplicado=dup(df);
    pos= fFindItem(df,ofList);
    item1 = fGetItem(pos, ofList);
    p= item1.fileName;
    sprintf (aux,"dup %d (%s)",df, p);
    fInsertItem(item1, &ofList);
    fcntl(duplicado,F_GETFL);
    printf("The file has been duplicated\n");
}


void Cmd_infosys (char *pcs[]) {
    struct utsname systemUname;
    if( uname(&systemUname) < 0 ) {
        perror("uname");
    } else {
        printf("System name: %s\nNodename: %s\nRelease: %s\nVersion: %s\nMachine: %s\n",
            systemUname.sysname, systemUname.nodename, systemUname.release, systemUname.version, systemUname.machine);
    }
}

struct command{
    char *name;
    void (*function) (char**);
};

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

void Cmd_quit (char *pcs[]){
    if (fIsEmptyList(ofList)) {
        fClearList(&ofList);
    }
    if (hIsEmptyList(hisList)) {
        hClearList(&hisList);
    }
    exit(0); //the program is terminating successfully
}





void Cmd_cwd (char *pcs[]){
    char dir[MAX];
    if (pcs[0]==NULL) printf("%s\n", getcwd(dir,MAX));
}

void Cmd_makefile (char *pcs[]){
    if (pcs[0]==NULL) {
        printf("The name of the file needs to be provided\n");
        return;
    }
    FILE *file = fopen(pcs[0], "r"); //fopen in mode "r" tries to open the file in read mode
    if (file!=NULL){ //fopen could open the file so that means the file already exists
        printf("Thhis file: %s already exists\n", pcs[0]);
        return;
    }
    file = fopen(pcs[0], "w"); //create the file in writer mode
    if (file==NULL) { //fopen fails
        printf("The file could not be created: %s\n", strerror(errno));
        return;
    }

    printf("%s was created\n", pcs[0]);

    fclose(file); //we just want to create the file
}