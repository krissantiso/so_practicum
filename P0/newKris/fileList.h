#ifndef FILELIST_H
#define FILELIST_H

#include <stdbool.h>

typedef char *fFileName;

typedef struct fNode* fPos;

typedef struct fItem {
    fFileName fileName;
    int fileDescriptor;
    int fileMode;
} fItem;

struct fNode{
    fItem data;
    fPos next;
};

typedef fPos fList;


bool fCreateNode(fPos* p);

void fCreateEmptyList(fList *L);

bool fInsertItem(fItem item, fList *L);

void fDeleteAtPosition(fPos pos, fList* L);

void fDeleteAll(fList *L);

bool fIsEmptyList(fList L);

fPos fFirst(fList L);

fPos fLast(fList L);

fPos fNext(fPos pos, fList L);

fPos fPrevious(fPos pos, fList L);

fItem fGetItem(fPos pos, fList L);

fPos fFindItem(fFileName name, fList L);

#endif //FILELIST_H
