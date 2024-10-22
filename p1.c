#include "commands.h"
#include "historicList.h"
#include "fileList.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/utsname.h>
#include <errno.h>
#include <fcntl.h>

hList hisList; //initialize historic list
fList ofList; //initialize open files list

struct CMD{ //external variables get initialized while compiling, so declaring it out of DoCommand is faster.
    char *name;
    void (*function) (char **);
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
    	{"makefile",Cmd_makefile},
	    {"makedir",Cmd_makedir},
    	{"listfile",Cmd_listfile},
    	{"cwd",Cmd_cwd},
    	{"listdir",Cmd_listdir},
    	{"reclist",Cmd_reclist},
    	{"revlist",Cmd_revlist},
    	{"erase",Cmd_erase},
    	{"delrec",Cmd_delrec},
        {NULL,NULL}
};

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

void DoCommand (char *pcs[]) {
    int i; // automatic variable
    if (pcs[0] == NULL)
        return;

    for (i = 0; C[i].name != NULL; i++) {
        if (strcmp(pcs[0], C[i].name) == 0) {  // Corrected the strcmp condition
            (*C[i].function)(pcs + 1);  // Call the corresponding function with pcs+1 as arguments
            return;
        }
    }
    printf("Command %s not found\n", pcs[0]);  // If no command matched
}

int main(int argc, char * argv []){
    char line[MAX];
    char *pcs[MAX/2];
    char command[MAX];

    hCreateEmptyList(&hisList);

    fCreateEmptyList(&ofList);
    fInitializeList(&ofList);

    while (1){
        printf ("-> ");
        fgets(line,MAX,stdin);
        strcpy(command, line);

        if (breakLine(line,pcs)==0) {
            continue;
        }
        if (!hInsertItem(command, &hisList)) {
            printf("There has been an error when adding to historic\n");
        }

        DoCommand(pcs);

    }
}