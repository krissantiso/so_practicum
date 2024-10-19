//
// Created by crist on 07/10/2024.
//

#ifndef P0_P1_H
#define P0_P1_H
#define MAX 2048
#include "historic_list.h"
#include "open_list.h"

extern tList HLIST;
extern tListO OFLIST;

void Cmd_makefile (char *pcs[]);
void Cmd_makedir (char *pcs[]);
void Cmd_listfile (char *pcs[]);
void Cmd_cwd (char *pcs[]);
void Cmd_listdir (char *pcs[]);
void Cmd_reclist (char *pcs[]);
void Cmd_revlist (char *pcs[]);
void Cmd_erase (char *pcs[]);
void Cmd_delrec (char *pcs[]);

#endif //P0_P1_H
