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
#include <sys/stat.h>
#include <dirent.h>

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

void Cmd_makedir (char *pcs[]){

}
void Cmd_listfile (char *pcs[]){

}


//THIS IS AN AUXILIARY FUNCTION TO PRINT INFORMATION ABOUT FILES
void print_file_info(const char *path, const struct stat *file_stat, int islong, int isacc, int islink) {
    // if -long flag is set, print detailed information
    if (islong) {
        printf((S_ISDIR(file_stat->st_mode)) ? "d" : "-");
        printf((file_stat->st_mode & S_IRUSR) ? "r" : "-");
        printf((file_stat->st_mode & S_IWUSR) ? "w" : "-");
        printf((file_stat->st_mode & S_IXUSR) ? "x" : "-");
        printf((file_stat->st_mode & S_IRGRP) ? "r" : "-");
        printf((file_stat->st_mode & S_IWGRP) ? "w" : "-");
        printf((file_stat->st_mode & S_IXGRP) ? "x" : "-");
        printf((file_stat->st_mode & S_IROTH) ? "r" : "-");
        printf((file_stat->st_mode & S_IWOTH) ? "w" : "-");
        printf((file_stat->st_mode & S_IXOTH) ? "x " : "- ");

        // display size
        printf("%ld ", file_stat->st_size);

        // display time, either access time (-acc) or modification time (default)
        struct tm *time_info;
        if (isacc)
            time_info = localtime(&file_stat->st_atime);
        else
            time_info = localtime(&file_stat->st_mtime);

        char time_str[256];
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", time_info);
        printf("%s ", time_str);
    }

    // Print the file or directory name
    printf("%s", path);

    // If -link flag is set and it's a symbolic link, print the target path
    if (islink && S_ISLNK(file_stat->st_mode)) {
        char link_target[1024];
        ssize_t len = readlink(path, link_target, sizeof(link_target) - 1);
        if (len != -1) {
            link_target[len] = '\0';  // Null-terminate the link target
            printf(" -> %s", link_target);
        }
    }

    printf("\n");
}


void Cmd_listdir (char *pcs[]){
    int i, ishid, islong, isacc, islink;
    ishid = islong = isacc = islink = 0;

    if (pcs[1] == NULL) {
        Cmd_cwd(pcs);  // just prints cwd
        return;
    }


    for (i = 0; pcs[i] != NULL; i++) { //need to know what to print
        if (strcmp(pcs[i], "-hid") == 0) ishid = 1; //hidden files
        else if (strcmp(pcs[i], "-long") == 0) islong = 1; //long list
        else if (strcmp(pcs[i], "-link") == 0) islink = 1; //if symbolic link -> content path
        else if (strcmp(pcs[i], "-acc") == 0) isacc = 1; //access time
        else break;  // now it is the dir name
    }

    //LIST CONTENTS
    for (; pcs[i] != NULL; i++) {
        DIR *dir = opendir(pcs[i]);  // Open directory
        if (dir == NULL) {
            printf("Cannot list %s: %s\n", pcs[i], strerror(errno));
            continue;
        }

        printf("\nDirectory: %s\n", pcs[i]);

        struct dirent *entry; //dirent represents directory entry
        while ((entry = readdir(dir)) != NULL) {
            char path[1024];
            // Build the full path to the file
            snprintf(path, sizeof(path), "%s/%s", pcs[i], entry->d_name);

            // Skip the special directories "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            // check for hidden files if -hid is not set to not print them
            if (!ishid && entry->d_name[0] == '.') {
                continue;  // skip hidden files
            }

            // if -long=1, show detailed information
            if (islong) {
                struct stat file_stat;
                if (stat(path, &file_stat) == 0) {
                    print_file_info(path, &file_stat, islong, isacc, islink);
                } else {
                    printf("Error getting info for %s: %s\n", entry->d_name, strerror(errno));
                }
            }
        }
        closedir(dir);  //close the directory
    }
}
void Cmd_reclist (char *pcs[]){
    int i, ishid, islong, isacc, islink;
    ishid = islong = isacc = islink = 0;

    // check if any directory arguments are provided
    if (pcs[1] == NULL) {
        Cmd_cwd(pcs);  // just prints current working directory
        return;
    }

    // checks what needs to be printed
    for (i = 1; pcs[i] != NULL; i++) { //start from 1 to skip the command name
        if (strcmp(pcs[i], "-hid") == 0) ishid = 1; // Hidden files
        else if (strcmp(pcs[i], "-long") == 0) islong = 1; // Long list
        else if (strcmp(pcs[i], "-link") == 0) islink = 1; // If symbolic link -> content path
        else if (strcmp(pcs[i], "-acc") == 0) isacc = 1; // Access time
        else break;  // Now it is the directory name
    }

    // Now `i` points to the directory name
    for (; pcs[i] != NULL; i++) {
        DIR *dir = opendir(pcs[i]); // Open directory
        if (dir == NULL) {
            printf("Cannot open directory %s: %s\n", pcs[i], strerror(errno));
            continue;
        }

        printf("\nDirectory: %s\n", pcs[i]);

        struct dirent *entry;
        struct stat file_stat;

        while ((entry = readdir(dir)) != NULL) {
            char path[1024];

            // Build the full path to the file
            snprintf(path, sizeof(path), "%s/%s", pcs[i], entry->d_name);

            // Skip the special directories "." and ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            // Skip hidden files unless -hid is specified
            if (!ishid && entry->d_name[0] == '.')
                continue;

            // Use lstat() to get file information (without following symbolic links)
            if (lstat(path, &file_stat) == -1) {
                printf("Error getting info for %s: %s\n", path, strerror(errno));
                continue;
            }

            // Print the file information based on the specified flags
            print_file_info(path, &file_stat, islong, isacc, islink);

            // If the file is a directory, call Cmd_reclist recursively
            if (S_ISDIR(file_stat.st_mode)) {
                char *sub_args[] = { pcs[0], pcs[i], NULL }; // Create a new argument array for the subdirectory
                Cmd_reclist(sub_args);  // Recursive call for the subdirectory
            }
        }

        closedir(dir); // Close the directory
    }
}
void Cmd_revlist (char *pcs[]){

}
void Cmd_erase (char *pcs[]){

}
void Cmd_delrec (char *pcs[]){

}