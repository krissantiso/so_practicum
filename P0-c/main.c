//
// Created by crist on 07/10/2024.
//



#include "p0.h"
#include "p1.h"
#include "open_list.h"
#include "historic_list.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/utsname.h>
#include <errno.h>
#include <fcntl.h>

tList HLIST; //initialize historic list
tListO OFLIST; //initialize open files list

struct CMD{ //external variables get initialized while compiling, so declaring it out of DoCommand is faster.
    char *name;
    void (*func) (char **);
};

struct CMD C[]={ //array of structures: i dont need to declare the dimension, bc i give initial values so it is automatic. 1st structure, 2nd struc
        {"authors", Cmd_authors}, //name of the command and function that does the command (it is a pointer to a function)
        {"pid", Cmd_pid},
        {"ppid", Cmd_ppid},
        {"cd", Cmd_cd},
        {"date", Cmd_date},
        {"historic", Cmd_historic},
        {"open", Cmd_open},
        {"close", Cmd_close},
        {"dup", Cmd_dup},
        {"infosys", Cmd_infosys},
        {"help", Cmd_help},
        {"quit", Cmd_quit},
        {"exit", Cmd_quit},
        {"bye", Cmd_quit},
        {NULL,NULL}
};

int BreakLine (char *lin, char *pz[]){
    int i;

    if ((pz[0]=strtok(lin, " \t\n"))==NULL) return 0;
    i=1;
    while ((pz[i]=strtok(NULL, " \t\n"))!=NULL)
        i++;
    return i;
}

bool DoCommand (char *pcs[]) {
    int i; // automatic variable
    if (pcs[0] == NULL)
        return false;

    for (i = 0; C[i].name != NULL; i++) {
        if (strcmp(pcs[0], C[i].name) == 0) {  // Corrected the strcmp condition
            (*C[i].func)(pcs + 1);  // Call the corresponding function with pcs+1 as arguments
            return true;
        }
    }

    printf("Command %s not found\n", pcs[0]);  // If no command matched
    return false;
}

int main(int argc, char * argv []){
    char line[MAX];
    char *pcs[MAX/2];
    char aux[MAX];
    createEmptyList(&HLIST);
    createEmptyListO(&OFLIST);
    while (1){
        printf ("->");
        fgets(line,MAX,stdin);
        strcpy(aux, line);
        if (BreakLine(line,pcs)>0)
            if (DoCommand(pcs)) {
                if (insertItem(aux, &HLIST)) {
                    continue;
                } else printf("There has been an error when adding to historic\n");
            }
    }
}