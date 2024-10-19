#ifndef HISTORICLIST_H
#define HISTORICLIST_H
#include <stdbool.h>

typedef char *hCommand;

typedef struct hNode* hPosL;

typedef struct hItemL {
    int commandNumber;
    hCommand command;
} hItemL;

struct hNode{
    hItemL data;
    hPosL next;
};

typedef hPosL hList;

bool hIsEmptyList(hList L);

bool hCreateNode(hPosL* p);

void hCreateEmptyList(hList *L);

bool hInsertItem(hItemL item, hList *L);

void hDeleteAll(hList *L);

hPosL hFirst(hList L);

hPosL hLast(hList L);

hPosL hNext(hPosL pos, hList L);

hPosL hPrevious(hPosL pos, hList L);

hItemL hGetItem(hPosL pos, hList L);

#endif