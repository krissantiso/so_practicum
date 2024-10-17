//
// Created by crist on 18/09/2024.
//

#ifndef P0_HISTORIC_LIST_H
#define P0_HISTORIC_LIST_H
#include <stdbool.h>
#define LNULL NULL

typedef struct tNode* tPosL;

struct tNode{
    char *cmd;
    tPosL next;
};

typedef tPosL tList;


void printList(tList L);
void printNElems(tList L, int n);

void createEmptyList(tList* L);

bool isEmptyList(tList L); //the list itself bc we don't intend to modify it

bool insertItem (char * newcommand, tList* L);


void clearList (tList *L);


char * getItem (int n, tList L);







#endif //P0_HISTORIC_LIST_H
