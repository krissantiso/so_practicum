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


//Practice 0 commands

void Cmd_authors (char *pcs[]){
    if ( pcs[0] == NULL ) { //if nothing requested
        printf("Kris Santiso Sampedro\tkris.santiso@udc.es\nCristina Vazquez Varela\tc.vazquezv@udc.es\n"); //print everything
    } else if ( strcmp( pcs[0], "-l" ) == 0 ) { //-l asks for mails
        printf("kris.santiso@udc.es\nc.vazquezv@udc.es\n"); //print mails
    } else if ( strcmp( pcs[0], "-n" ) == 0) { //-n asks for names
        printf("Kris Santiso Sampedro\nCristina Vazquez Varela\n"); //print names
    } else {
        printf("Invalid argument\n"); //if requested something invalid
    }
}

void Cmd_pid (){
    printf("Process ID: %d\n", getpid()); //process id of current process
}

void Cmd_ppid (){
    printf("Parent process ID: %d\n", getppid()); //parent process id of current process
}

void Cmd_cd (char *pcs[]) {
    if ( pcs[0]==NULL ) { //if nothing requested
        char dir[MAX]; //char array to store cwd
        if (getcwd(dir, sizeof(dir)) != NULL) { //if call succeeds
            printf("%s\n", dir); //print cwd
        } else {
            printf("Cannot do. Error number is %d (%s)\n", errno, strerror(errno)); //if call fails, prints error
        }
    } else { //directory requested
        if ( chdir(pcs[0]) != 0 ) { //changes dir to path specified
            printf("Cannot do. Error number is %d (%s)\n", errno, strerror(errno)); //if it doesn't work, print error
        }
    }
}

void Cmd_date (char *pcs[]) {
    time_t t;
    time(&t); //retrieves current time and stores it in t

    struct tm * time = localtime(&t); //changes time retrieved to represent it in local time and store it in time

    if (pcs[0]==NULL) { //if nothing requested
        printf("%.2d %.2d %.2d\n%.2d:%.2d:%.2d\n", time->tm_mday, time->tm_mon+1, time->tm_year + 1900, time->tm_hour, time->tm_min, time->tm_sec); //print everything
    } else if ( strcmp( pcs[0], "-d") == 0 ) { //-d asks for date
        printf("%.2d %.2d %.4d\n", time->tm_mday, time->tm_mon+1, time->tm_year + 1900);
    } else if ( strcmp( pcs[0], "-t") == 0 ) { //-t asks for time
        printf("%.2d:%.2d:%.2d\n", time->tm_hour, time->tm_min, time->tm_sec);
    } else {
        printf("Invalid argument %s\n", pcs[0]); //if requested something invalid
    }
}

void Cmd_historic(char *pcs[]) {
    int n; //just to store number requested

    if (pcs[0] == NULL) { //if nothing requested
        hPrintList(hisList); //prints the whole list
        return;
    }
    n = atoi(pcs[0]); //change the number given (char) to an int
    if (n==0){ //if the number is 0 we cannot print anything
        printf("Please insert a valid number\n");
        return;
    }
    if (n>0) { //that means the command is historic N
        char *line; //helps finding the command
        line = hGetItem(n, hisList); //gets the item in position n
        if(line==NULL){ //if it is not found
            printf("There is no command at this position\n");
            return;
        }
        if (breakLine(line,pcs)>0) {
            DoCommand(pcs); //repeats the command
        }
    }else { //just to ensure the number is valid
        n = abs(atoi(pcs[0])); //makes n positive because of format historic -N
        hPrintNElems(n, hisList); //prints from last element to the nth element
        return;
    }
}

void Cmd_open (char *pcs[]){
    if ( pcs[0] == NULL ){ //if nothing requested
        fPrintList(ofList); //just prints the list of open files
        return;
    }
    fItemL item; //auxiliar to hold file descriptor info
    int i, df, mode = 0; //i is for iteration, df for file descriptor and mode for open files modes

    for (i=1; pcs[i]!=NULL; i++){ //iterates through argument to check requested modes
        if (!strcmp(pcs[i],"cr")) mode|=O_CREAT;
        else if (!strcmp(pcs[i],"ex")) mode|=O_EXCL;
        else if (!strcmp(pcs[i],"ro")) mode|=O_RDONLY;
        else if (!strcmp(pcs[i],"wo")) mode|=O_WRONLY;
        else if (!strcmp(pcs[i],"rw")) mode|=O_RDWR;
        else if (!strcmp(pcs[i],"ap")) mode|=O_APPEND;
        else if (!strcmp(pcs[i],"tr")) mode|=O_TRUNC;
        else break;
    }

    if ((df=open(pcs[0],mode,0777))==-1) { //tries to open the file with the specified mode and permissions 0777
        perror ("Impossible to open file"); //if it cannot be opened -> fail
    } else {

        item.fileDescriptor = df; //store file descriptor
        strcpy(item.fileName, pcs[0]); //copy file name

        if ( fInsertItem(item, &ofList) ) {
            printf("Added entry %d to the open files table \n", item.fileDescriptor); //success message
        } else {
            printf("There has been an error trying to add the file\n"); //fail message
        }

    }
}

void Cmd_close(char *pcs[]){
    int df; //file descriptor
    if (pcs[0]==NULL || (df=atoi(pcs[0]))<0) { //if nothing requested or df is invalid
        printf("Argument invalid or inexistent\n"); //error message
        return;
    }
    if (close(df)==-1) { //tries to close descriptor
        perror("Impossible to close descriptor"); //prints error
        return;
    }
    if(fIsEmptyList(ofList)) { //if the list of open files is empty
        printf("There are no open files\n"); //we cannot close a file that is not open
        return;
    }
    fPosL p = fFirst(ofList); //start at the beginning of the list
    while ( p != NULL ) { //iterate through the list
        fItemL item = fGetItem(p, ofList); //get the item in position p
        if (item.fileDescriptor == df) { //if the file descriptor coincide
            fDeleteAtPosition(p, &ofList); //deletes that position of the list
            return;
        }
        p = fNext(p, ofList); //"increment" the position
    }
}


void Cmd_dup (char * tr[])
{
    int df; //file descriptor
    char aux[MAX],*p; //aux as a buffer and p is a pointer to file name
    fPosL pos; //position auxiliar
    fItemL item1; //auxiliar to find the right file
    if (tr[0]==NULL || (df=atoi(tr[0]))<0) { //there is no parameter or the descriptor is invalid
        printf("Argument must be a valid file descriptor\n");
        return;
    }

    if ( dup(df) == -1) { //attempts to duplicate the fd
        printf("Cannot duplicate. Error is number %d (%s)\n", errno, strerror(errno)); //fail message
        return;
    }

    pos = fFindItem(df,ofList); //finds item with file descriptor = df
    item1 = fGetItem(pos, ofList); //gets item in postion pos
    p = item1.fileName; //stores file name
    sprintf (aux,"dup %d (%s)",df, p); //prepares entry with that format
    fItemL item2;
    strcpy(item2.fileName, aux); //stores the entry in item2

    int descriptor = fLastDescriptor(ofList) + 1; //gets the last fd of the list and adds one, so it will be the new last one
    item2.fileDescriptor = descriptor;; //assign the previous descriptor to item2

    fInsertItem(item2, &ofList); //insert the dup of the file

    printf("The file has been duplicated\n"); //success message
}


void Cmd_infosys (char *pcs[]) {
    struct utsname systemUname; //declare a structure utsname to hold the system information such as system name, node name, release, version, and machine type
    if( uname(&systemUname) < 0 ) { //calls uname to get system info and store it in systemUname
        perror("uname"); //if it fails, print error message
    } else { //print system information
        printf("System name: %s\nNodename: %s\nRelease: %s\nVersion: %s\nMachine: %s\n",
            systemUname.sysname /*name of OS*/, systemUname.nodename /*name of the machine within the network (hostname)*/,
            systemUname.release /*OS release (e.g., kernel version)*/,systemUname.version /*specific version of OS*/,
            systemUname.machine /*hardware type (architecture)*/);
    }
}

void Cmd_quit (char *pcs[]){
    if ( !fIsEmptyList(ofList)) { //if the list of files is not empty
        fClearList(&ofList); //clear the list before exiting
    }
    if ( !hIsEmptyList(hisList)) { //if the historic list is not empty
        hClearList(&hisList); //clear list
    }
    exit(0); //the program is terminating successfully
}



//Practice 1 commands

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
    return (pw == NULL) ? "???????" : pw->pw_name; //if it couldn't access to info, prints the user as unknown (???)
    //otherwise prints username
}

char *print_group(struct stat buffer) { //same as print_owner but with group
    int group = buffer.st_gid;
    struct group *gr = getgrgid(group);
    return (gr == NULL) ? "???????" : gr->gr_name;
}

void print_link(struct stat buffer, char *fName){
    char toLink[MAX] = "";
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

void printFile(char *fName, int isLong, int isLink, int isAcc, int isHid) {
    if (!(!isHid && fName[0] == '.')) { //skip hidden files if isHid=0 and if the current file starts with '.'
        char *perm;
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
                    print_link(buffer, fName);
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
                   print_link(buffer, fName);
                }
                printf("\n");
            }
        }
    }
}

void printLISTDIR(char *dirName, int isLong, int isLink, int isAcc, int isHid){
    char dir[MAX];
    getcwd(dir, MAX); //get cwd and store it in dir. MAX is passed so the path doesn't exceed the buffer size
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
                    if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0){//ensure that we skip listing dirs . and .. in the recursion
                        if (ent->d_name[0] == '.' && !isHid) {
                            continue;  // skip hidden subdirectories if -hid was not requested
                        }

                        if (isRev) { //reverse recursive requested
                            printREC(ent->d_name, isLong, isLink, isAcc, isHid, isRec, isRev); //recursion first
                            printLISTDIR(ent->d_name, isLong, isLink, isAcc, isHid); //list directory
                        } else {
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

void Cmd_cwd (char *pcs[]){
    char dir[MAX]; //char array to store path of current directory
    if (pcs[0]==NULL){
        if (getcwd(dir,MAX)!=NULL) { //gets cwd
            printf("%s\n", dir); //if succeeds, print dir path
        }else {
            perror("Error getting current working directory"); //print error message if getcwd fails
        }
    }
    else printf("Invalid argument to this command\n"); //prints error message
}

void Cmd_makefile (char *pcs[]){
    if (pcs[0]==NULL) { //if no name provided
        printf("The name of the file needs to be provided\n"); //prints fail message
        return;
    }
    int df = open(pcs[0], O_CREAT|O_TRUNC|O_WRONLY|O_EXCL, 0777);
    //attempts to create a new file with the name provided using the open() system call
    /*O_CREAT: creates the file if it doesn't exist
     *O_TRUNC: if the file exists, truncate its size to 0
     *O_WRONLY: open for write-only mode
     *O_EXCL: ensures that the call fails if the file already exists */

    if ( df == -1 ) { //checks if open failed
		printf("Cannot create. Error number is %d (%s)\n", errno, strerror(errno)); //error message
        return;
    }
    printf("%s was created\n", pcs[0]); //success message
    close(df); //closes fd to ensure resources are properly released
}

void Cmd_makedir (char *pcs[]){
	if (pcs[0]==NULL) { //if no name provided
        printf("The name of the directory needs to be provided\n"); //prints fail message
        return;
    }
    int ok = mkdir(pcs[0], 0777); //0 if success, non-zero if fail
    //attempt to create a new directory with the name provided
    //0777: read, write, and execute permissions for everyone
	if ( ok != 0 ) { //if it failed
    	printf("Cannot create. Error number is %d (%s)\n", errno, strerror(errno)); //error message
        return;
	}
    printf("%s was created\n", pcs[0]); //success message
}

struct Flags get_flags(char *pcs[], int *dirStart) {
    struct Flags flags = {0, 0, 0, 0}; // initialize all flags to 0
    int j; //position of arguments


    for (j = 0; pcs[j] != NULL; j++) { //checks for options through the command
        if (strcmp(pcs[j], "-hid") == 0) flags.isHid = 1;
        else if (strcmp(pcs[j], "-long") == 0) flags.isLong = 1;
        else if (strcmp(pcs[j], "-link") == 0) flags.isLink = 1;
        else if (strcmp(pcs[j], "-acc") == 0) flags.isAcc = 1;
        else break; //stop when it's no longer an option -> dir/file
    }

    *dirStart = j; //pass back the position of the first dir/file argument
    return flags;  //return the structure with the flags
}

void Cmd_listfile (char *pcs[]){
    if (pcs[0]==NULL) { //checks if the name is not provided
        printf("The name of the directory or file needs to be provided\n"); //error message
        return;
    }
    int dirStart; //this will hold the position of the first dir/file argument
    struct Flags flags=get_flags(pcs, &dirStart); //auxiliary function to get arguments
    flags.isHid=1; //we can print hidden files with this command

    for (int i = dirStart; pcs[i] != NULL; i++) { //iterates through different files given
    	struct stat stats; //auxiliary to hold file info
    	if ( lstat(pcs[i], &stats) == 0) { //get file info and store it in stats
            printFile(pcs[i], flags.isLong, flags.isLink, flags.isAcc, flags.isHid); //print file info
   		}else { //if lstat fails
            printf("Cannot get stats of %s. Error number is %d (%s)\n", pcs[i], errno, strerror(errno)); //error message
   		}
    }
}

void Cmd_listdir(char *pcs[]) {


    if (pcs[0]==NULL){ //if no name provided
        printf("The name of the directory must be specified\n"); //error message
        return;
    }
    int dirStart; //this will hold the position of the first dir/file argument
    struct Flags flags=get_flags(pcs, &dirStart); //auxiliary function to get arguments

        for (int i = dirStart; pcs[i] != NULL; i++) { //iterates through different directories given
            struct stat buffer; //initialize stat to hold info
            if( lstat(pcs[i], &buffer)==0){ //get info of directory or file
                if(S_ISDIR(buffer.st_mode)) { //check if it is a directory
                    printLISTDIR(pcs[i], flags.isLong, flags.isLink, flags.isAcc, flags.isHid); //list contents of directory
                }else
                    printFile(pcs[i], flags.isLink, flags.isLong, flags.isAcc, flags.isHid); //prints file info
            }else printf("Cannot get stats of %s. Error number is %d (%s)\n", pcs[i], errno, strerror(errno)); //error message
        }
}


void Cmd_reclist (char *pcs[]){
    int isRec=1, isRev=0;


    if (pcs[0]==NULL){ //if no name provided
        printf("The name of the directory must be specified\n"); //error message
        return;
    }

    int dirStart; //this will hold the position of the first dir/file argument
    struct Flags flags=get_flags(pcs, &dirStart); //auxiliary function to get arguments

    for (int i = dirStart; pcs[i] != NULL; i++) { //iterates through different directories given
        struct stat buffer; //initialize stat to hold info
        if( lstat(pcs[i], &buffer)==0){ //get info of directory or file
            if(S_ISDIR(buffer.st_mode)) { //check if it is a directory
                printLISTDIR(pcs[i], flags.isLong, flags.isLink, flags.isAcc, flags.isHid); //list contents of directory
                printREC(pcs[i], flags.isLong, flags.isLink, flags.isAcc, flags.isHid, isRec, isRev); //then make recursion of subdirs inside
            }else
                printFile(pcs[i], flags.isLink, flags.isLong, flags.isAcc, flags.isHid); //prints file info
        }else printf("Cannot get stats of %s. Error number is %d (%s)\n", pcs[i], errno, strerror(errno)); //error message
    }
}
void Cmd_revlist (char *pcs[]){
    int isRec=0, isRev=1;

    if (pcs[0]==NULL){ //if no name provided
        printf("The name of the directory must be specified\n"); //error message
        return;
    }

    int dirStart; //this will hold the position of the first dir/file argument
    struct Flags flags=get_flags(pcs, &dirStart); //auxiliary function to get arguments

    for (int i = dirStart; pcs[i] != NULL; i++) { //iterates through different directories given
        struct stat buffer; //initialize stat to hold info
        if( lstat(pcs[i], &buffer)==0){ //get info of directory or file
            if(S_ISDIR(buffer.st_mode)) { //check if it is a directory
                printREC(pcs[i], flags.isLong, flags.isLink, flags.isAcc, flags.isHid, isRec, isRev); //first make recursion of subdirs inside
                printLISTDIR(pcs[i], flags.isLong, flags.isLink, flags.isAcc, flags.isHid); //list contents of directory
            }else
                printFile(pcs[i], flags.isLink, flags.isLong, flags.isAcc, flags.isHid); //prints file info
        }else printf("Cannot get stats of %s. Error number is %d (%s)\n", pcs[i], errno, strerror(errno)); //error messsage
    }
}

void Cmd_erase (char *pcs[]){
    if (pcs[0]==NULL){ //if no name provided
        printf("The name of the file or directory must be specified\n"); //error message
        return;
    }
    for ( int i = 0; pcs[i] != NULL; i++) { //this loop goes through all provided files/dirs names
        if (remove(pcs[i]) != 0 ) { //called to delete the file/dir
            printf("Cannot delete %s. Error number is %d (%s)\n", pcs[i], errno, strerror(errno));
            //if fails (returns non-zero value) -> print error message
        } else {
            printf("%s removed\n", pcs[i]); //success message
        }
    }
}

void auxDel (char *path) {
    struct dirent *de;
    DIR *dr = opendir(path); //attempts to open the directory at path
    // if it succeeds, it returns a pointer to the directory stream (DIR *dr)
    if( dr == NULL) { //if opendir fails
        printf("No file or directory found (%s)\n", path); //error message
        return;
    }
    while((de = readdir(dr)) != NULL) { //this loop goes through each entry in directory to read contents
        char newDir[MAX];
        sprintf(newDir,"%s/%s",path,de->d_name);
        //for each file or subdir, it concatenates the current path and the dir entry name (de->d_name) into newDir
        if ( strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0 ) { //ensure cwd and parent dir are ignored
            continue;
        }
        if ( remove(newDir) != 0 ) { //try to delete current dir entry (newDir)
            //if remove fails: entry is likely a subdirectory
            auxDel(newDir); //recursion to delete contents of the subdir
        }
    }
    closedir(dr); //after all entries have been processed -> dir is closed
    if ( remove(path) != 0 ) { //remove directory itself
        printf("Cannot delete %s: Error number is %d (%s)\n", path, errno, strerror(errno)); //if fails -> error message
    }
}

void Cmd_delrec (char *pcs[]){
    if (pcs[0]==NULL){ //if no name provided
        printf("The name of the directory must be especified\n"); //error message
        return;
    }
    char cwd[MAX];
    getcwd(cwd, MAX); //stores current working directory in cwd
    for ( int i = 0; pcs[i] != NULL; i++) { //goes through all the dir names provided
        if ( remove(pcs[i]) == 0 ) { //attempts to delete the dir directly
            printf("%s deleted\n", pcs[i]); //if succeeded -> success message
            continue;
        }
        if ( chdir(pcs[i]) != 0 ) { //if fails -> tries to change to the provided dir
            printf("Cannot do. Error number is %d (%s)\n", errno, strerror(errno)); //error message
            continue;
        }
        //change dir successfully
        char dir[MAX];
        getcwd(dir, MAX); //gets the path of the directory to be removed and stores it in dir
        auxDel(dir); //initialize recursive deletion
        chdir(cwd); //changes the dir back to cwd: it ensures that the program returns to its  original dir
    }
}