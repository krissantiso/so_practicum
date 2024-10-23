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

void print_crtime(struct stat buffer){
    time_t ct;
    struct tm tm;
    ct = buffer.st_ctime; //gets modification time of file
    localtime_r(&ct, &tm); //converts it to local time structure
    printf("%04d/%02d/%02d-%02d:%02d ", (tm.tm_year + 1900), (tm.tm_mon + 1), tm.tm_mday, tm.tm_hour, tm.tm_min);
    //print atributres change time (creation)
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

        if (lstat(fName, &buffer) != 0) { //get file information
            printf("It is not possible to access %s: %s\n", fName, strerror(errno)); //error if lstat fails
        } else {
            if (!isLong) { //if long not requested
                printf("%d ", print_size(buffer)); //just print size
                if (isAcc) { //if access time requested
                    print_accesstime(buffer); //prints acc.time
                }
                printf("%s", fName); //also print file name
                if (isLink) { //if link requested
                    if (LetraTF(buffer.st_mode) == 'l') { //checks if the file is a symbolic link
                        ssize_t len = readlink(fName, toLink, sizeof(toLink) - 1); //readlink reads the target of the link
                        if (len != -1) {
                            toLink[len] = '\0';  // null-terminate the string
                            if (S_ISLNK(buffer.st_mode)) { //check again if the file is a symbolic link
                                printf(" -> %s", toLink); //print link target
                            } else {
                                printf("\n"); //if it is not a symb.link just prints new line
                            }
                        } else {
                            printf("Error reading link for %s: %s", fName, strerror(errno)); //if readlink fails
                        }
                    }
                }
                printf("\n");
            } else { //if long requested
                if (isAcc) { //if access time requested
                    print_accesstime(buffer); //prints acc.time
                } else {
                    print_crtime(buffer); //prints creation or modification time
                }
                perm = (char *)malloc(12); //allocate mem for permissions
                if (perm == NULL) { //if allocation fails
                    fprintf(stderr, "Memory allocation failed\n"); //print error message
                    return;  // Handle allocation failure
                }
                ConvierteModo(buffer.st_mode, perm); //converts mode to permissions
                printf(" %2d (%8d) %8s %8s %14s %6d %s",
                print_hardlinks(buffer), print_inoden(buffer),
                       print_owner(buffer), print_group(buffer),
                       perm, print_size(buffer), fName); //print all info
                free(perm); //free memory allocated for permissions
                if (isLink) { //if link is requested
                    if (LetraTF(buffer.st_mode) == 'l') { //same as before
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
                    } else printf("\n");

                } else {
                    printf("\n");
                }
            }
        }
    }
}

void printLISTDIR(char *dirName, int isLong, int isLink, int isAcc, int isHid){
    char dir[MAX];
    getcwd(dir, MAX); //get cwd and store it in dir
    struct stat buffer;
    struct stat buffer2;
    DIR * dirc;
    struct dirent *ent;

    if(lstat(dirName, &buffer) == -1) //get info about the directory
        printf("It is not possible to access %s: %s\n", dirName, strerror(errno)); //if lstat fails
    else if (!S_ISREG(buffer.st_mode) && !S_ISLNK(buffer.st_mode)){ //if it is neither a regular file nor symb.link
        if ((dirc = opendir(dirName)) == NULL) //try to open directory
            printf("It is not possible to access %s: %s\n", dirName, strerror(errno)); //if it can't be opened
        else{
            if (chdir(dirName) == -1) { //change to target directory
                printf("Cannot change to directory %s: %s\n", dirName, strerror(errno)); //error if failed to change
                closedir(dirc); //then close the directory
                return;
            }

            printf("** %s **\n", dirName); //print the directory name

            while ((ent = readdir (dirc)) != NULL){ //with this loop we will read each entry in the directory

                lstat(ent->d_name, &buffer2); //get info for each entry
                printFile(ent->d_name, isLong, isLink, isAcc, isHid); //prints info of entry

            }
            closedir(dirc); //close the directory
        }
        chdir(dir); //change back to cwd
    }
}

void printREC(char *fName, int isLong, int isLink, int isAcc, int isHid, int isRec, int isRev){
    //everything is the same as in printLISTDIR except for the prints
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
                        if(isRev){ //reverse recursive requested
                            printREC(ent->d_name, isLong, isLink, isAcc, isHid, isRec, isRev); //recursion first
                            printLISTDIR(ent->d_name, isLong, isLink, isAcc, isHid); //list directory
                        }
                        else{
                            printLISTDIR(ent->d_name, isLong, isLink, isAcc, isHid); //list directory
                            printREC(ent->d_name, isLong, isLink, isAcc, isHid, isRec, isRev); //then recursion

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
    int df, duplicate;
    char aux[MAX],*p;
    fPosL pos;
    fItemL item1; //auxiliar to find the right file
    if (tr[0]==NULL || (df=atoi(tr[0]))<0) { //there is no parameter or the descriptor is less than 0
        fPrintList(ofList);
        return;
    }

    duplicate=dup(df);
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
    int isLong=0, isLink=0, isAcc=0, i;

    for ( i = 0; pcs[i] != NULL; i++) {
    	if (strcmp(pcs[i], "-long") == 0) isLong = 1;
        else if (strcmp(pcs[i], "-link") == 0) isLink = 1;
        else if (strcmp(pcs[i], "-acc") == 0) isAcc = 1;
        else break; //i is the name location
    }

    for (i = i; pcs[i] != NULL; i++) {
    	struct stat stats;
    	if ( lstat(pcs[i], &stats) == 0) {
            printFile(pcs[i], isLong, isLink, isAcc, 1);
   		}else {
            printf("Cannot get stats of %s. Error number is %d (%s)\n", pcs[i], errno, strerror(errno));
   		}
    }


}

void Cmd_listdir(char *pcs[]) {
    int i,j;
    int isLong, isAcc , isHid , isLink;
    isLong=isAcc=isHid=isLink=0;

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
    if (pcs[0]==NULL){
        printf("The name of the file or directory must be especified");
        return;
    }
    for ( int i = 0; pcs[i] != NULL; i++) {
        if (remove(pcs[i]) != 0 ) {
            printf("Cannot delete %s. Error number is %d (%s)\n", pcs[i], errno, strerror(errno));
        } else {
            printf("%s removed\n", pcs[i]);
        }
    }
}

void auxDel (char *path) {
    struct dirent *de;
    DIR *dr = opendir(path);
    if( dr == NULL) {
        printf("No file or directory found (%s)\n", path);
        return;
    }
    while((de = readdir(dr)) != NULL) {
        char newDir[MAX];
        sprintf(newDir,"%s/%s",path,de->d_name);
        if ( strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0 ) {
            continue;
        }
        if ( remove(newDir) != 0 ) {
            auxDel(newDir);
        }
    }
    closedir(dr);
    if ( remove(path) != 0 ) {
        printf("Cannot delete %s: Error number is %d (%s)\n", path, errno, strerror(errno));
    }
}

void Cmd_delrec (char *pcs[]){
    if (pcs[0]==NULL){
        printf("The name of the directory must be especified\n");
        return;
    }
    char cwd[MAX];
    strcpy(cwd, getcwd(cwd, MAX));
    for ( int i = 0; pcs[i] != NULL; i++) {
        if ( remove(pcs[i]) == 0 ) {
            printf("%s deleted\n", pcs[i]);
            continue;
        }
        if ( chdir(pcs[i]) != 0 ) {
            printf("Cannot do. Error number is %d (%s)\n", errno, strerror(errno));
            continue;
        }
        char dir[MAX];
        strcpy(dir, getcwd(dir, MAX));
        auxDel(dir);
        chdir(cwd);
    }
}