//
// Created by kris on 10/20/24.
//

#include "commands.h"
#include "historicList.h"
#include "fileList.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>


char LetraTF(mode_t m) {
    switch (m & S_IFMT) {
        case S_IFSOCK: return 's'; //checks if it is a socket
        case S_IFLNK: return 'l'; //if symbolic link
        case S_IFREG: return '-'; //if regular file
        case S_IFBLK: return 'b'; //if block device
        case S_IFDIR: return 'd'; //if directory
        case S_IFCHR: return 'c'; //if character device
        case S_IFIFO: return 'p'; //if FIFO
        default: return '?'; //if unknown
    }
}

char *ConvierteModo(mode_t m, char *permisos) {
    strcpy(permisos, "---------- "); //sets format to substitute each position by permission
    permisos[0] = LetraTF(m);
    if (m & S_IRUSR) permisos[1] = 'r'; //user read, write and execute
    if (m & S_IWUSR) permisos[2] = 'w';
    if (m & S_IXUSR) permisos[3] = 'x';
    if (m & S_IRGRP) permisos[4] = 'r'; //group read write and execute
    if (m & S_IWGRP) permisos[5] = 'w';
    if (m & S_IXGRP) permisos[6] = 'x';
    if (m & S_IROTH) permisos[7] = 'r'; //others read write and execute
    if (m & S_IWOTH) permisos[8] = 'w';
    if (m & S_IXOTH) permisos[9] = 'x';
    if (m & S_ISUID) permisos[3] = 's'; //set-user-ID-bit
    if (m & S_ISGID) permisos[6] = 's'; //set-group-ID-bit
    if (m & S_ISVTX) permisos[9] = 't'; //sticky-bit
    return permisos;
}

int print_size(struct stat buffer){
    return buffer.st_size; //size of file
}

int print_inoden(struct stat buffer){
    return buffer.st_ino; //inode number of file
}

int print_hardlinks(struct stat buffer){
    return buffer.st_nlink; //number of hardlinks to the file
}

void print_modtime(struct stat buffer){
    time_t mt;
    struct tm tm;
    mt = buffer.st_mtime; //gets modification time of file
    localtime_r(&mt, &tm); //converts it to local time structure
    printf("%04d/%02d/%02d-%02d:%02d ", (tm.tm_year + 1900), (tm.tm_mon + 1), tm.tm_mday, tm.tm_hour, tm.tm_min);
    //print modification time
}

void print_accesstime(struct stat buffer){
    time_t at;
    struct tm tm;
    at = buffer.st_atime; //gets access time of file
    localtime_r(&at, &tm);
    printf("%04d/%02d/%02d-%02d:%02d ", (tm.tm_year + 1900), (tm.tm_mon + 1), tm.tm_mday, tm.tm_hour, tm.tm_min);
}

char *print_owner(struct stat buffer) {
    int owner = buffer.st_uid; //gets owner's user ID
    struct passwd *pw = getpwuid(owner); //looks up user info
    return (pw == NULL) ? "???????" : pw->pw_name; //if couldn't access to info, prints the user as unknown (???)
    //otherwise prints user name
}

char *print_group(struct stat buffer) { //same as print_owner but with group
    int group = buffer.st_gid;
    struct group *gr = getgrgid(group);
    return (gr == NULL) ? "???????" : gr->gr_name;
}

void printFile(char *fName, int isLong, int isLink, int isAcc, int isHid) {
    if (!(!isHid && fName[0] == '.')) { //skip hidden files if isHid=0 and if the current file starts with '.'
        char toLink[MAX] = "", *perm;
        struct stat buffer;

        if (lstat(fName, &buffer) == -1) { //get file information
            printf("It is not possible to access %s: %s\n", fName, strerror(errno)); //error if lstat fails
        } else {
            if (!isLong && !isAcc) { //if neither long nor acc was requested
                printf("%d %s\n", print_size(buffer), fName);
            } else {
                if (isAcc) {
                    print_accesstime(buffer);
                } else {
                    print_modtime(buffer);
                }
                perm = (char *)malloc(12);
                if (perm == NULL) {
                    fprintf(stderr, "Memory allocation failed\n");
                    return;  // Handle allocation failure
                }
                ConvierteModo(buffer.st_mode, perm);
                printf(" %2d (%8d) %8s %8s %14s %6d %s",
                print_hardlinks(buffer), print_inoden(buffer),
                       print_owner(buffer), print_group(buffer),
                       perm, print_size(buffer), fName);
                free(perm);
                if (isLink) {
                    ssize_t len = readlink(fName, toLink, sizeof(toLink) - 1);
                    if (len != -1) {
                        toLink[len] = '\0';  // Null-terminate the string
                        if (S_ISLNK(buffer.st_mode)) {
                            printf(" -> %s\n", toLink);
                        } else {
                            printf("\n");
                        }
                    } else {
                        printf("Error reading link for %s: %s\n", fName, strerror(errno));
                    }
                } else {
                    printf("\n");
                }
            }
        }
    }
}

void printLISTDIR(char *dirName, int isLong, int isLink, int isAcc, int isHid){
    char dir[MAX];
    getcwd(dir, MAX);
    struct stat buffer;
    struct stat buffer2;
    DIR * dirc;
    struct dirent *ent;

    if(lstat(dirName, &buffer) == -1)
        printf("It is not possible to access %s: %s\n", dirName, strerror(errno));
    else if (!S_ISREG(buffer.st_mode) && !S_ISLNK(buffer.st_mode)){
        if ((dirc = opendir(dirName)) == NULL)
            printf("It is not possible to access %s: %s\n", dirName, strerror(errno));
        else{
            if (chdir(dirName) == -1) {
                printf("Cannot change to directory %s: %s\n", dirName, strerror(errno));
                closedir(dir);
                return;
            }

            printf("************%s\n", dirName);

            while ((ent = readdir (dirc)) != NULL){
                if (!isLong && !isAcc){
                    lstat(ent->d_name, &buffer2);
                    if(isHid)
                        printf("%d %s\n", print_size(buffer2), ent->d_name);
                    else if(ent->d_name[0] != '.')
                        printf("%d %s\n", print_size(buffer2), ent->d_name);
                }else if(isLong || isAcc)
                    printFile(ent->d_name, isLong, isLink, isAcc, isHid);

            }
            closedir(dirc);
        }
        chdir(dir);
    }
}

void printREC(char *fName, int isLong, int isLink, int isAcc, int isHid, int isRec, int isRev){

    if(!isHid && fName[0] == '.')
        return;

    char dir[MAX];
    getcwd(dir, MAX);
    struct stat buffer;
    DIR * dirc;
    struct dirent *ent;

    if(lstat(fName, &buffer) == -1)
        printf("It is not possible to access %s: %s\n", fName, strerror(errno));
    else{
        if(S_ISDIR(buffer.st_mode)){
            if ((dirc = opendir(fName)) != NULL){
                chdir(fName);
                while ((ent = readdir (dirc)) != NULL){
                    if(strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")){
                        if(isRev){
                            printREC(ent->d_name, isLong, isLink, isAcc, isHid, isRec, isRev);
                            printLISTDIR(ent->d_name, isLong, isLink, isAcc, isHid);
                        }
                        else{
                            printLISTDIR(ent->d_name, isLong, isLink, isAcc, isHid);
                            printREC(ent->d_name, isLong, isLink, isAcc, isHid, isRec, isRev);

                        }
                    }
                }
                closedir(dirc);
            }
            chdir(dir);
        }
    }
}

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
        printf("%.2d %.2d %.2d\n%.2d:%.2d:%.2d\n", time->tm_mday, time->tm_mon+1, time->tm_year + 1900, time->tm_hour, time->tm_min, time->tm_sec);
    } else if ( strcmp( pcs[0], "-d") == 0 ) {
        printf("%.2d %.2d %.4d\n", time->tm_mday, time->tm_mon+1, time->tm_year + 1900);
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
        DoCommand(pcs);
    }
}

//me gusto mas mi funcion asi q la use y ya :p
void Cmd_historic(char *pcs[]) {
    int n;

    if (pcs[0] == NULL) {
        hPrintList(hisList);
        return;
    }
    n = atoi(pcs[0]);
    if (n==0){
        printf("Please insert a valid number\n");
        return;
    }
    if (n>0) {
        char *line; //helps finding the command
        line = hGetItem(n, hisList);
        if(line==NULL){
            printf("There is no command at this position\n");
            return;
        }
        if (breakLine(line,pcs)>0) {
            DoCommand(pcs);
        }
    }else { //just to ensure the number is valid
        n = abs(atoi(pcs[0]));
        hPrintNElems(n, hisList);
        return;
    }
}

void Cmd_open (char *pcs[]){
    if ( pcs[0] == NULL ){
        fPrintList(ofList);
        return;
    }
    fItemL item;
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

        item.fileDescriptor = df;
        strcpy(item.fileName, pcs[0]);

        if ( fInsertItem(item, &ofList) ) {
            printf("Added entry %d to the open files table \n", item.fileDescriptor);
        } else {
            printf("There has been an error trying to add the file\n");
        }

    }
}

/*
 * void Cmd_makefile (char *tr[]){
 *  int df;
 *  if (tr[0]==NULL){
 *      lkjdslkjas
 *      return;
*   }
 *  if ((df=open(tr[0], O_CREAT|O_EXCL, 0777))==-1){
 *      mnt emur ------
 *      return;
 *  }
 *  close(df);
 * }
 *
 *
 *
 * Cmd_listfile(){
 * int l, islong, islnk, isacc;
 * islong=islnk=isacc=0;
 * for (i=0; tr[i]!=NULL;i++){
 *  if(!strcmp(tr[i], "-long") islong=1;
 *  else
 *  .
 *  .
 *  .
 *  else break
 * }
 *  for(;tr[i]!=NULL;i++){
 *      fileInfo(tr[i],islong,islnk,isacc);
 *  }
 * }
 *
 *
 * fileInfo(ch *file, int islong, int islnk, int isacc){
 *  struct stat s;
 *      if(lstat(file, &s)==-1){
 *          ldakjf
 *          return;
 *      }
 *      if(!ishid){
 *
 * }
 */

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

    duplicado = dup(df);
    pos = fFindItem(df,ofList);
    item1 = fGetItem(pos, ofList);
    p = item1.fileName;
    sprintf (aux,"dup %d (%s)",df, p);
    fItemL item2;
    strcpy(item2.fileName, aux);

    int descriptor = fLastDescriptor(ofList) + 1;
    item2.fileDescriptor = descriptor;;

    fInsertItem(item2, &ofList);

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

void Cmd_quit (char *pcs[]){
    if ( !fIsEmptyList(ofList)) {
        fClearList(&ofList);
    }
    if ( !hIsEmptyList(hisList)) {
        hClearList(&hisList);
    }
    exit(0); //the program is terminating successfully
}



//Practice 1 commands


void Cmd_cwd (char *pcs[]){
    char dir[MAX];
    if (pcs[0]==NULL) printf("%s\n", getcwd(dir,MAX));
}

void Cmd_makefile (char *pcs[]){
    if (pcs[0]==NULL) {
        printf("The name of the file needs to be provided\n");
        return;
    }
    int df = open(pcs[0], O_CREAT|O_TRUNC|O_WRONLY|O_EXCL, 0777);
    if ( df == -1 ) {
		printf("Cannot create. Error number is %d (%s)\n", errno, strerror(errno));
        return;
    }
    printf("%s was created\n", pcs[0]);
    close(df);
}

void Cmd_makedir (char *pcs[]){
	if (pcs[0]==NULL) {
        printf("The name of the directory needs to be provided\n");
        return;
    }
    int ok = mkdir(pcs[0], 0777);
	if ( ok != 0 ) {
    	printf("Cannot create. Error number is %d (%s)\n", errno, strerror(errno));
        return;
	}
    printf("%s was created\n", pcs[0]);
}

void Cmd_listfile (char *pcs[]){

    if (pcs[0]==NULL) {
        printf("The name of the directory needs to be provided\n");
        return;
    }
    int isLong=0, isLink=0, isAcc=0, i, isHid=0;

    for ( i = 0; pcs[i] != NULL; i++) {
    	if (strcmp(pcs[i], "-long") == 0) isLong = 1;
        else if (strcmp(pcs[i], "-link") == 0) isLink = 1;
        else if (strcmp(pcs[i], "-acc") == 0) isAcc = 1;
        else break; //i is the name location
    }

    for (i = i; pcs[i] != NULL; i++) {
    	struct stat stats;
    	if ( lstat(pcs[i], &stats) == 0) {
            printFile(pcs[i], isLink, isLong, isAcc, isHid);
   		}else {
            printf("Cannot get stats of %s. Error number is %d (%s)\n", pcs[i], errno, strerror(errno));
   		}
    }


}

void Cmd_listdir(char *pcs[]) {
    char dir[MAX];
    int i,j;
    int isLong, isAcc , isHid , isLink;
    isLong=isAcc=isHid=isLink;

    if (pcs[0]==NULL){
        Cmd_cwd(pcs);
        return;
    }

    for(j = 0; pcs[j] != NULL; j++){
        if (strcmp(pcs[j], "-hid") == 0) isHid = 1;
        else if (strcmp(pcs[j], "-long") == 0) isLong = 1;
        else if (strcmp(pcs[j], "-link") == 0) isLink = 1;
        else if (strcmp(pcs[j], "-acc") == 0) isAcc = 1;
        else break; //i is the name location
    }

        for (i = j; pcs[i] != NULL; i++) {
            struct stat buffer;
            if( lstat(pcs[i], &buffer)==0){
                if(S_ISDIR(buffer.st_mode)) { //check if it is a directory
                    printLISTDIR(pcs[i], isLong, isLink, isAcc, isHid);
                }else
                    printFile(pcs[i], isLink, isLong, isAcc, isHid);
            }else printf("Cannot get stats of %s. Error number is %d (%s)\n", pcs[i], errno, strerror(errno));

        }

}


void Cmd_reclist (char *pcs[]){
    char dir[MAX];
    int i,j;
    int isLong, isAcc , isHid , isLink, isRec, isRev;
    isRec=1;
    isLong=isAcc=isHid=isLink=isRev=0;

    if (pcs[0]==NULL){
        Cmd_cwd(pcs);
        return;
    }

    for(j = 0; pcs[j] != NULL; j++){
        if (strcmp(pcs[j], "-hid") == 0) isHid = 1;
        else if (strcmp(pcs[j], "-long") == 0) isLong = 1;
        else if (strcmp(pcs[j], "-link") == 0) isLink = 1;
        else if (strcmp(pcs[j], "-acc") == 0) isAcc = 1;
        else break; //i is the name location
    }

    for (i = j; pcs[i] != NULL; i++) {
        struct stat buffer;
        if( lstat(pcs[i], &buffer)==0){
            if(S_ISDIR(buffer.st_mode)) { //check if it is a directory

                printLISTDIR(pcs[i], isLong, isLink, isAcc, isHid);
                printREC(pcs[i], isLong, isLink, isAcc, isHid, isRec, isRev);
            }else
                printFile(pcs[i], isLink, isLong, isAcc, isHid);
        }else printf("Cannot get stats of %s. Error number is %d (%s)\n", pcs[i], errno, strerror(errno));
    }
}
void Cmd_revlist (char *pcs[]){
    char dir[MAX];
    int i,j;
    int isLong, isAcc , isHid , isLink, isRec, isRev;
    isRev=1;
    isLong=isAcc=isHid=isLink=isRec=0;

    if (pcs[0]==NULL){
        Cmd_cwd(pcs);
        return;
    }

    for(j = 0; pcs[j] != NULL; j++){
        if (strcmp(pcs[j], "-hid") == 0) isHid = 1;
        else if (strcmp(pcs[j], "-long") == 0) isLong = 1;
        else if (strcmp(pcs[j], "-link") == 0) isLink = 1;
        else if (strcmp(pcs[j], "-acc") == 0) isAcc = 1;
        else break; //i is the name location
    }

    for (i = j; pcs[i] != NULL; i++) {
        struct stat buffer;
        if( lstat(pcs[i], &buffer)==0){
            if(S_ISDIR(buffer.st_mode)) { //check if it is a directory
                printREC(pcs[i], isLong, isLink, isAcc, isHid, isRec, isRev);
                printLISTDIR(pcs[i], isLong, isLink, isAcc, isHid);
            }else
                printFile(pcs[i], isLink, isLong, isAcc, isHid);
        }else printf("Cannot get stats of %s. Error number is %d (%s)\n", pcs[i], errno, strerror(errno));
    }
}
void Cmd_erase (char *pcs[]){

}
void Cmd_delrec (char *pcs[]){

}