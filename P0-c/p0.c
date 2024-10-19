/* STUDENTS' INFORMATION:
 * Cristina Vázquez Varela: c.vazquezv@udc.es
 * Kris Santiso Sampedro: kris.santiso@udc.es
 * Group 6.1
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/utsname.h>
#include <errno.h>
#include <fcntl.h>
#include "p0.h"
#include "historic_list.h"
#include "open_list.h"




void Cmd_authors (char *pcs[]){
    if (pcs[0]==NULL) printf("Cristina Vázquez Varela -> c.vazquezv@udc.es\nKris Santiso Sampedro -> kris.santiso@udc.es\n");
    else if(!strcmp(pcs[0], "-n"))
        printf("Cristina Vázquez Varela \nKris Santiso Sampedro\n");
    else if (!strcmp(pcs[0], "-l"))
        printf("c.vazquezv@udc.es \nkris.santiso@udc.es\n");
}
void Cmd_pid (){
    printf("pid: %d\n", getpid());
}

void Cmd_ppid (){
    printf("ppid: %d\n", getppid());
}

void Cmd_cd (char *pcs[]){
    char dir[MAX];
    if (pcs[0]==NULL) printf("%s\n", getcwd(dir,MAX));
    else if (chdir(pcs[0])==-1) { //checks if the change of directory was successful (returns 0) or not (returns -1)
        printf("Cannot do. Error number is %d (%s)\n", errno, strerror(errno));
    }
}

void Cmd_date (char *pcs[]){
    time_t t= time(NULL);
    struct tm date = *localtime(&t);
    if (pcs[0]==NULL) printf("%02d/%02d/%04d\n%02d:%02d:%02d\n", date.tm_mday, date.tm_mon + 1, date.tm_year + 1900, date.tm_hour, date.tm_min, date.tm_sec);
    else if (!strcmp(pcs[0], "-t"))
        printf("%02d:%02d:%02d\n", date.tm_hour, date.tm_min, date.tm_sec);
    else if (!strcmp(pcs[0], "-d"))
        printf("%02d/%02d/%04d\n", date.tm_mday, date.tm_mon + 1, date.tm_year + 1900);
}

void Cmd_historic (char *pcs[]) {
    int n;

    if (pcs[0] == NULL) {
        printList(HLIST);
        return;
    }
    n = atoi(pcs[0]);
    if (n==0){
        printf("Please insert a valid number\n");
        return;
    }
    if (n>0) {
        char *line; //helps finding the command
        line = getItem(n, HLIST);
        if(line==NULL){
            printf("There is no command at this position\n");
            return;
        }
        if (BreakLine(line,pcs)>0) {
            if(DoCommand(pcs)) return;
        }
    }else { //just to ensure the number is valid
        n = abs(atoi(pcs[0]));
        printNElems(HLIST, n);
        return;
    }


}

 void Cmd_open (char * pcs[]) {
    int i, df, mode = 0;
    tItemL file;

    if (pcs[0] == NULL) { //there is no parameter
        printListO(OFLIST);
        return;
    }
    for (i = 1; pcs[i] != NULL; i++)
        if (!strcmp(pcs[i], "cr")) mode |= O_CREAT;
        else if (!strcmp(pcs[i], "ex")) mode |= O_EXCL;
        else if (!strcmp(pcs[i], "ro")) mode |= O_RDONLY;
        else if (!strcmp(pcs[i], "wo")) mode |= O_WRONLY;
        else if (!strcmp(pcs[i], "rw")) mode |= O_RDWR;
        else if (!strcmp(pcs[i], "ap")) mode |= O_APPEND;
        else if (!strcmp(pcs[i], "tr")) mode |= O_TRUNC;
        else break;

    if ((df = open(pcs[0], mode, 0777)) == -1)
        perror("Impossible to open file\n");

    else {
        file.des=df;
        file.mode=mode;
        file.name= strdup(pcs[0]);
        if(insertItemO(file, &OFLIST)) {
            printf("Entry added to the Open Files table..................\n"/*, ....*/);
            return;
        }else printf("There has been an error trying to add the file\n");

    }
}
void Cmd_close (char *tr[]){
    int df;
    tPosLO pos;
     if (tr[0]==NULL || (df=atoi(tr[0]))<0) { //there is no parameter or the descriptor is less than 0
        printListO(OFLIST);
        return;
    }

     pos= findItemO(df, OFLIST);

    if (close(df)==-1)
        perror("Impossible to close file");
    else{
        if(isEmptyListO(OFLIST))
            printf("There are no open files\n");
        else{
            deleteAtPositionO(pos, &OFLIST);
            printf("The file with descriptor %d is being closed......\n", df);
            return;
        }
    }

}

void Cmd_dup (char * tr[])
{
    int df, duplicado;
    char aux[MAX],*p;
    tPosLO pos;
    tItemL item1; //auxiliar to find the right file
    if (tr[0]==NULL || (df=atoi(tr[0]))<0) { //there is no parameter or the descriptor is less than 0
        printListO(OFLIST);
        return;
    }

    duplicado=dup(df);
    pos= findItemO(df,OFLIST);
    item1 = getItemO(pos, OFLIST);
    p= item1.name;
    sprintf (aux,"dup %d (%s)",df, p);
    insertItemO(item1, &OFLIST);
    fcntl(duplicado,F_GETFL);
    printf("The file has been duplicated\n");
}

void Cmd_infosys (char *pcs[]){
    struct utsname unamedata;
    uname(&unamedata);
    printf("System name: %s\nVersion: %s\nRelease: %s\nMachine: %s\nNode name: %s\n", unamedata.sysname, unamedata.version,
           unamedata.release, unamedata.machine, unamedata.nodename);
}

void Cmd_help (char *pcs[]){
    if (pcs[0]==NULL) {
        printf("authors [-l|-n]\npid\nppid\ncd [dir]\ndate [-t|-d]\nhistoric [N|-N]\nopen [file] mode\n"
               "close [df]\ndup [df]\ninfosys\nhelp [cmd]\nquit\nbye\nexit\n");
        return;
    }else {
        if (!strcmp(pcs[0], "authors"))
            printf("Prints the names and logins of the program authors. authors -l prints "
                   "only the logins and authors -n prints only the names.\n");
        else if (!strcmp(pcs[0], "pid"))
            printf("Prints the pid of the process executing the shell.\n");
        else if (!strcmp(pcs[0], "ppid"))
            printf ("Prints the pid of the shell’s parent process.\n");
        else if (!strcmp(pcs[0], "cd"))
            printf("cd [dir] Changes the current working directory of the shell to dir (using the "
                   "chdir system call). When invoked without auguments it prints the 1 "
                   "current working directory\n");
        else if (!strcmp(pcs[0], "date"))
            printf("Prints the current date in the format DD/MM/YYYY and the current time in the format hh:mm:ss.\n"
                   "date -d Prints the current date in the format DD/MM/YYYY\n"
                   "date -t Prints and the current time in the format hh:mm:ss.\n");
        else if (!strcmp(pcs[0], "historic"))
            printf("Shows the historic of commands executed by this shell. In order to "
                   "do this, a list to store all the commands input to the shell must be "
                   "implemented.\n"
                   "historic Prints all the commands that have been input with their order number\n"
                   "historic N Repeats command number N (from historic list)\n"
                   "historic -N Prints only the lastN commands\n");
        else if (!strcmp(pcs[0], "open"))
            printf("open [file] mode Opens a file and adds it (together with the file descriptor and the "
                   "opening mode to the list of shell open files. For the mode we’ll use cr for "
                   "O CREAT, ap for O APPEND, ex for O EXCL, ro for O RDONLY, "
                   "rw for O RDWR, wo for O WRONLY and tr for O TRUNC.\n"
                   "Open without arguments lists the shell open files. For each file it lists its "
                   "descriptor, the file name and the opening mode. The shell will inherit "
                   "from its parent process open descriptors 0, 1 and 2 (stdin, stout, and "
                   "stderr).\n");
        else if (!strcmp(pcs[0], "close"))
            printf("close [df ] Closes the df file descriptor and eliminates the corresponding item from the list\n");
        else if (!strcmp(pcs[0], "dup"))
            printf("dup [df ] Duplicates the df file descriptor (using the dup system call, creating the "
                   "corresponding new entry on the file list\n");
        else if (!strcmp(pcs[0], "help"))
            printf("help [cmd] help displays a list of available commands. help cmd gives a brief help "
                   "on the usage of command cmd\n");
        else if (!strcmp(pcs[0], "quit") || !strcmp(pcs[0], "bye") || !strcmp(pcs[0], "exit"))
            printf ("quit, bye and exit End the shell\n");
    }
}

void Cmd_quit (char *pcs[]){
    printf("You are exiting the program\n");
    clearListO(&OFLIST);
    clearList(&HLIST);
    exit(0); //the program is terminating successfully
}



