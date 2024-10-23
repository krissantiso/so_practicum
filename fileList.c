#include "fileList.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAX 2048

fPosL fFirst(fList L){
    //the first position is the position L points to
    return (L);
}

//function that returns the last position of a given list
fPosL fLast(fList L){
    fList p=L;
    //it searches for the last position by iterating until the positions next is null and returns it
    while (p->next != NULL) {
        p = p->next;
    }
    return p;
}

int fLastDescriptor(fList L){
    fList p=L;
    //it searches for the last position by iterating until the positions next is null
    while (p->next != NULL) {
        p = p->next;
    }
    fItemL item = fGetItem(p, L);
    return item.fileDescriptor;
}

//function that returns the next position of a given position
fPosL fNext(fPosL pos, fList L){
    //it returns the next position, if its the last it returns LNULL by definition
    return pos->next;
}

//function that returns the previous position of a given position
fPosL fPrevious(fPosL pos, fList L){
    fList p=L;
    if (pos == L){
        //if the position is the first one then it doesnt have a previous position
        return NULL;
    }else {
        //it searches for the previous position by iterating until p next is equal to the given position
        while (p->next != pos) {
            p = p->next;
        }
        return p;
    }
}

fItemL fGetItem (fPosL p, fList L){
    return p->data;
}

void fPrintList(fList L){
    fPosL pos;
    fItemL item;

    if (!fIsEmptyList(L)) {
        pos = fFirst(L);
        while (pos != NULL) {
            item = fGetItem(pos, L);

            int iMode = fcntl(item.fileDescriptor, F_GETFL);
            char sMode[MAX];
            strcpy(sMode, "");
            if (iMode & O_WRONLY) strcpy(sMode, "- O_WRONLY");
            else if (iMode & O_RDWR) strcpy(sMode, "- O_RDWR");
            else if (iMode & O_APPEND) strcpy(sMode, "- O_APPEND");

            printf(" - %d > %s %s\n", item.fileDescriptor, item.fileName, sMode);
            pos = fNext(pos, L);
        }
    }
    else printf("There are no open files\n");
}
void fCreateEmptyList(fList* L){
    *L = NULL;
}

void fInitializeList(fList* L){
    fItemL item;
    int i;
    for ( i = 0; fcntl(i, F_GETFL) != -1; i++ ) {
        item.fileDescriptor = i;
        switch (i){
            case 0: strcpy(item.fileName, "standart input"); break;
            case 1: strcpy(item.fileName, "standart output"); break;
            case 2: strcpy(item.fileName, "standart error"); break;
            default: strcpy(item.fileName, "null"); break;
        }
        fInsertItem(item, L);
    }
}

bool fIsEmptyList(fList L){
    return (L == NULL);
}

bool createNode(fPosL* p){
    *p = malloc(sizeof(struct fNode));
    return *p != NULL;
}

bool fInsertItem (fItemL d, fList* L){
    fPosL p = *L, q;
    //if the node creating fails it returns false and stops the insertion
    if (!createNode(&q)) {
        return false;
    } else {
        //the item to insert is initialised
        q->data = d;
        q->next = NULL;
        //if the list is empty
        if (*L == NULL) {
            //the list now points to the new item thus adding it at the start
            *L = q;
        }

        else {
            //it searches for the last position
            while (p->next != NULL) {
                p = p->next;
            }
            //the last item now points to the new item thus adding it at the end
            p->next = q;
        }


    }
    return true;
}

fPosL fFindItem (int d, fList L){
    fPosL p;
    for(p=L; (p!= NULL) && (p->data.fileDescriptor!=d);p=p->next);
    return p;
}

fPosL fDeleteAtPosition (fPosL p, fList* L){
    fPosL q;
    if (p == *L) {
        *L = (*L) -> next;
    } else if (p->next == NULL) {//delete lost element
        for (q = *L; q->next != p; q = q->next);//find position in front of p
        q->next = NULL;
    } else {//delete from the middle
        q = p->next;
        p->data = q->data;
        p->next = q->next;
        p = q; //get rid of the node that used to be right after p
    }
    return p;
    free(p);
}

void fClearList (fList *L){
    fPosL p=*L;
    fPosL next;
    if (fIsEmptyList(*L)) return;
    else {
        while (p != NULL) {
            next = p->next; //saves the next node
            free(p); //deletes the current one
            p=next; //advances
        }
    }
}