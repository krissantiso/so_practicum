#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


#define MAX 2048
#define LNULL NULL

// LIST STUFF!!

// DATA TYPES

typedef char * tCommand[MAX];




typedef struct tNode* tPosL;

struct tNode {
    tCommand command;
    tPosL next;
};

typedef tPosL tList;

// FUNCTIONS

//Generators
bool createNode(tPosL* p){
    *p=malloc(sizeof(struct tNode));
    return *p!=LNULL;
}

void createEmptyList(tList* L) {
    *L=LNULL;
}

bool insertItem(tCommand cmd, tList* L) {
    tPosL q,p=*L;
    if (!createNode(&q)) {
        return false;
    } else {
        *q->command=*cmd;
        q->next=LNULL;
        if (*L==LNULL) {
            *L=q;
        } else {
            while(p->next!=LNULL) {
                p=p->next;
            }
            p->next=q;
        }
    }
    return true;
}

//Destructors
bool deleteAll(tList* L) {
    if (*L==NULL) {
        return false;
    } else {
        tPosL p, q;
        p = *L;
        q = p->next;
        while (p != NULL) {
            free(p);
            p=q;
            if (q!=NULL) {
                q=q->next;
            }
        }
        *L = NULL;
        return true;
    }
}



//Observers
bool isEmptyList(tList L) {
    return (L==LNULL);
}


tCommand *getItem(tPosL pos, tList L) {
    return &pos->command;
}


tPosL findCommand(int n, tList L) {
    tPosL p;
    p=L;
    int i = 0;
    while (i != n) {
        if ((p->next)==LNULL) {
            return LNULL;
        }
        p = p->next;
        i++;
    }
    return p;
}

void printListN(int N, tList L) {

}

void printListFull(tList L) {
    tPosL p = L;
    int i = 0;
    while (p != NULL) {
        printf("%d->%s",i,*p->command);
        i++;
        p = p->next;
    }
}

// END OF LIST STUFF!!!!!

struct CMD {
    char *name;
    void (*function) (char**);
};

//COMMAND FUNCTIONS

void Cmd_authors(char *pz[]) {
    if (pz[0] == NULL) {
        printf("Diego Arturo Contreras Gonzalez\tdiego.contreras@udc.es\n"
               "Antia Vazquez Marino\t\tantia.vazquez@udc.es\n");
    } else if (strcmp(pz[0], "-n") == 0) {
        printf("Diego Arturo Contreras Gonzalez\nAntia Vazquez Marino\n");
    } else if (strcmp(pz[0], "-l") == 0) {
        printf("diego.contreras@udc.es\nantia.vazquez@udc.es\n");
    } else {
        printf("Invalid argument\n");
    }
}

struct helpOPTS {
    char *name;
    void (*function) (char**);
};


//HELP THINGS

void Cmd_help (char *pz[]) {
    if (pz[1] == NULL) {
        printf("authors: Prints the names and logins of the program authors. authors -l prints\n"
               "only the logins and authors -n prints only the names\n\n"
               "pid: Prints the pid of the process executing the shell.\n\n"
               "ppid: Prints the pid of the shell's parent process.\n\n"
               "cd [dir]: Changes the current working directory of the shell to dir When\n"
               "invoked without arguments it prints the current working directory.\n\n"
               "date [-t|-d]: Prints the current date in the format DD/MM/YYYY and the current\n"
               "time in the format hh:mm:ss.\n\n"
               "\tdate -d: Prints the current date in the format DD/MM/YYYY\n\n"
               "\tdate -t: Prints and the current time in the format hh:mm:ss.\n\n"
               "historic [N|-N]: Shows the historic of commands executed by this shell. In order to\n"
               "do this, a list to store all the commands input to the shell must be\n"
               "implemented.\n"
               "\t– historic: Prints all the commands that have been input with their\n"
               "order number\n"
               "\t– historic N: Repeats command number N (from historic list)\n"
               "\t– historic -N: Prints only the lastN commands.\n\n"
               "open [file] mode: Opens a file and adds it (together with the file descriptor and the\n"
               "opening mode to the list of shell open files. For the mode we’ll use cr for\n"
               "O CREAT, ap for O APPEND, ex for O EXCL, ro for O RDONLY,\n"
               "rw for O RDWR, wo for O WRONLY and tr for O TRUNC.\n"
               "Open without arguments lists the shell open files. For each file it lists its\n"
               "descriptor, the file name and the opening mode.\n\n"
               "close [df]: Closes the df file descriptor and eliminates the corresponding item from\n"
               "the list.\n\n"
               "dup [df]: Duplicates the df file descriptor.\n\n"
               "infosys: Prints information on the machine running the shell.\n\n"
               "help [cmd]: help displays a list of available commands. help cmd gives a brief help\n"
               "on the usage of command cmd.\n\n"
               "quit: Ends the shell.\n\n"
               "exit: Ends the shell.\n\n"
               "bye: Ends the shell.\n");
    }
}

void helpAuthors (char *pz[]) {
    printf("authors: Prints the names and logins of the program authors. authors -l prints\n"
           "only the logins and authors -n prints only the names\n");
}

void helpPid (char *pz[]) {
    printf("pid: Prints the pid of the process executing the shell.\n");
}

void helpPpid (char *pz[]) {
    printf("ppid: Prints the pid of the shell's parent process.\n");
}

void helpCd (char *pz[]) {
    printf("cd [dir]: Changes the current working directory of the shell to dir When\n"
           "invoked without arguments it prints the current working directory.\n");
}

void helpDate (char *pz[]) {
    printf("date [-t|-d]: Prints the current date in the format DD/MM/YYYY and the current\n"
           "time in the format hh:mm:ss.\n\n"
           "\tdate -d: Prints the current date in the format DD/MM/YYYY\n\n"
           "\tdate -t: Prints and the current time in the format hh:mm:ss.\n");
}

void helpHistoric (char *pz[]) {
    printf("historic [N|-N]: Shows the historic of commands executed by this shell. In order to\n"
           "do this, a list to store all the commands input to the shell must be\n"
           "implemented.\n"
           "\t– historic: Prints all the commands that have been input with their\n"
           "order number\n"
           "\t– historic N: Repeats command number N (from historic list)\n"
           "\t– historic -N: Prints only the lastN commands.\n");
}

void helpOpen (char *pz[]) {
    printf("open [file] mode: Opens a file and adds it (together with the file descriptor and the\n"
           "opening mode to the list of shell open files. For the mode we’ll use cr for\n"
           "O CREAT, ap for O APPEND, ex for O EXCL, ro for O RDONLY,\n"
           "rw for O RDWR, wo for O WRONLY and tr for O TRUNC.\n"
           "Open without arguments lists the shell open files. For each file it lists its\n"
           "descriptor, the file name and the opening mode.\n");
}

void helpClose (char *pz[]) {
    printf("close [df]: Closes the df file descriptor and eliminates the corresponding item from\n"
           "the list.\n");
}

void helpDup (char *pz[]) {
    printf("dup [df]: Duplicates the df file descriptor.\n");
}

void helpInfo (char *pz[]) {
    printf("infosys: Prints information on the machine running the shell.\n");
}

void helpHelp (char *pz[]) {
    printf("help [cmd]: help displays a list of available commands. help cmd gives a brief help\n"
           "on the usage of command cmd.\n");
}

void helpQuit (char *pz[]) {
    printf("quit: Ends the shell.\n");
}

void helpExit (char *pz[]) {
    printf("exit: Ends the shell.\n");
}

void helpBye (char *pz[]) {
    printf("bye: Ends the shell.\n");
}

struct helpOPTS options[] = {
        {"authors", helpAuthors},
        {"pid", helpPid},
        {"ppid", helpPpid},
        {"cd",helpCd},
        {"date",helpDate},
        {"historic", helpHistoric},
        {"open",helpOpen},
        {"close",helpClose},
        {"dup", helpDup},
        {"info",helpInfo},
        {"help",helpHelp},
        {"quit", helpQuit},
        {"exit", helpExit},
        {"bye", helpBye},
        {}

};

void Cmd_quit(char *pz[]) {

}

struct CMD C[]={
    {"authors",Cmd_authors},
    {"quit",Cmd_quit},
    {"exit",Cmd_quit},
    {"bye",Cmd_quit},
    {NULL,NULL},
};

int BreakLine (char *lin, char *pz[]) {
    if ((pz[0]=strtok(lin," \n\t"))==NULL)
        return 0;
    int i=1;
    while ((pz[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}













/* void ReadCommand (char *pz[]) {
    if (strcmp(pz[0], "authors") == 0) {
        showAuthors(pz);
    } else if (strcmp(pz[0], "help") == 0) {
        helpShell(pz);
    }
    else {
        printf("Invalid command\n");
    }
} */

void historicShell(char *pz[]) {
    if (strcmp(pz[0],"historic") == 0) {

    }
}

void DoCommand (char *pz[]) {
    int i;
    if (pz[0]==NULL)
        return;
    for (i=0;C[i].name!=NULL; i++)
        if (!strcmp(pz[0],C[i].name)){
            (*C[i].function)(pz+1);
            return;
        }
    printf ("Command %s not found\n",pz[0]);

}

int main () {
    char line [MAX];
    char *pcs [MAX/2];
    tList L;
    createEmptyList(&L);
    while (1) {
        printf ("#)");
        fgets(line,MAX,stdin);	//USA FGETS EN VEZ DE SCANF PARA PODER PONER ESPACIOS
        if (BreakLine(line,pcs)==0)
            continue;
        insertItem(pcs, &L);
        DoCommand(pcs);

    }
}

