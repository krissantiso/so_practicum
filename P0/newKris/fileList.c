#include "fileList.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool fCreateNode(fPos* p){
    *p = malloc(sizeof(struct fNode));
    return *p != NULL;
}

void fCreateEmptyList(fList *L){
    *L = NULL;
}

bool fInsertItem(fItem item, fList *L){
    fPos p = *L, q;
    if (!fCreateNode(&q)) {
        return false;
    } else {
        q->data = item;
        q->next = NULL;
        if (*L == NULL) {
            *L = q;
        }
        else {
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = q;
        }
    }
    return true;
}

void fDeleteAtPosition(fPos pos, fList* L){
    fList p = *L;
    if (pos == *L) {
        *L = pos->next;
    }else if (pos->next == NULL){
        while (p->next != pos) {
            p = p->next;
        }
        p->next = NULL;
    }else {
        while (p->next != pos) {
            p = p->next;
        }
        p->next = pos->next;
    }
    free(pos);
}

void fDeleteAll(fList *L){
    while (L != NULL) {
    	fDeleteAtPosition(*L, L);
    }
}

bool fIsEmptyList(fList L){
    return L == NULL;
}

fPos fFirst(fList L) {
    return L;
}

fPos fLast(fList L){
    fList p=L;
    while (p->next != NULL) {
        p = p->next;
    }
    return p;
}

fPos fNext(fPos pos, fList L){
    return pos->next;
}

fPos fPrevious(fPos pos, fList L){
    fList p=L;
    if (pos == L){
        return NULL;
    }else {
        while (p->next != pos) {
            p = p->next;
        }
        return p;
    }
}

fItem fGetItem(fPos pos, fList L){
    return pos->data;
}

fPos fFindItem(fFileName name, fList L){
    fList p = L;
    while (strcmp(p->data.fileName , name) != 0) {
        if (p->next == NULL){
            return NULL;
        }
        p = p->next;
    }
    return p;
}