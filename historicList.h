// Authors:
// Kris Santiso Sampedro    kris.santiso@udc.es
// Cristina Vazquez Varela  c.vazquezv@udc.es

#ifndef HISTORICLIST_H
#define HISTORICLIST_H

#include <stdbool.h>

typedef struct hNode* hPosL;

typedef char* command;

struct hNode{
    command command;
    hPosL next;
};

typedef hPosL hList;

void hPrintList(hList L);
void hPrintNElems(int n, hList L);

void hCreateEmptyList(hList *L);

bool hIsEmptyList(hList L);

bool hInsertItem(command newCommand, hList *L);

void hClearList (hList *L);

command hGetItem (int n, hList L);

#endif //HISTORICLIST_H
