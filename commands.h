// Authors:
// Kris Santiso Sampedro    kris.santiso@udc.es
// Cristina Vazquez Varela  c.vazquezv@udc.es

#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX 2048
#include <stdbool.h>
#include "historicList.h"
#include "fileList.h"

extern hList hisList;
extern fList ofList;

struct Flags {
    int isLong;
    int isAcc;
    int isHid;
    int isLink;
};

void Cmd_authors (char *pcs[]);
void Cmd_pid ();
void Cmd_ppid ();
void Cmd_cd (char *pcs[]);
void Cmd_date (char *pcs[]);
void Cmd_historic (char *pcs[]);
void Cmd_open (char * pcs[]);
void Cmd_close (char *tr[]);
void Cmd_dup (char * tr[]);
void Cmd_infosys (char *pcs[]);
void Cmd_quit (char *pcs[]);
int breakLine (char *lin, char *pz[]);
void DoCommand (char *pcs[]);

void Cmd_makefile (char *pcs[]);
void Cmd_makedir (char *pcs[]);
void Cmd_listfile (char *pcs[]);
void Cmd_cwd (char *pcs[]);
void Cmd_listdir (char *pcs[]);
void Cmd_reclist (char *pcs[]);
void Cmd_revlist (char *pcs[]);
void Cmd_erase (char *pcs[]);
void Cmd_delrec (char *pcs[]);

#endif //COMMANDS_H
