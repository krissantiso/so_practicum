//
// Created by crist on 07/10/2024.
//

#ifndef P0_P0_H
#define P0_P0_H
#define MAX 2048
#include <stdbool.h>
#include "historic_list.h"
#include "open_list.h"

extern tList HLIST;
extern tListO OFLIST;

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
void Cmd_help (char *pcs[]);
void Cmd_quit (char *pcs[]);
int BreakLine (char *lin, char *pz[]);
bool DoCommand (char *pcs[]);


#endif //P0_P0_H
