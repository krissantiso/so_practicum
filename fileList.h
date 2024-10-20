#ifndef FILELIST_H
#define FILELIST_H
#include <stdbool.h>

typedef char *fFileName;
typedef char *fFileMode;

typedef struct fNode* fPosL;

typedef struct fItemL {
    fFileName fileName;
    int fileDescriptor;
    fFileMode fileMode;
} fItemL;

struct fNode{
    fItemL data;
    fPosL next;
};

typedef fPosL fList;

fPosL fFirst(fList L);
fPosL fNext(fPosL pos, fList L);
void fPrintList(fList L);
void fCreateEmptyList(fList* L);
void fInitializeList(fList* L); //adds the already open files to the list
bool fIsEmptyList(fList L); //the list itself bc we don't intend to modify it
bool fInsertItem (fItemL d, fList* L);
fPosL fFindItem (int d, fList L);
fPosL fDeleteAtPosition (fPosL p, fList* L);
fItemL fGetItem (fPosL p, fList L);
void fClearList (fList *L);

#endif //FILELIST_H
