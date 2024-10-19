//
// Created by crist on 24/09/2024.
//

#ifndef P0_OPEN_LIST_H
#define P0_OPEN_LIST_H
#include <stdbool.h>
#define LNULL NULL

typedef struct tItemL{
    char *name;
    int des;
    int mode;
}tItemL;

typedef struct tNodeO* tPosLO;

struct tNodeO{
    tItemL file;
    tPosLO next;
};

typedef tPosLO tListO;


void printListO(tListO L);
void createEmptyListO(tListO* L);
bool isEmptyListO(tListO L); //the list itself bc we don't intend to modify it
bool insertItemO (tItemL d, tListO* L);
tPosLO findItemO (int d, tListO L);
tPosLO deleteAtPositionO (tPosLO p, tListO* L);
tItemL getItemO (tPosLO p, tListO L);
void clearListO (tListO *L);


#endif //P0_OPEN_LIST_H
