#include "historicList.h"
#include <stdlib.h>
#include <stdbool.h>

bool hCreateNode(hPosL* p){
    *p = malloc(sizeof(struct hNode));
    return *p != NULL;
}

void hCreateEmptyList(hList *L){
    *L = NULL;
}

bool hInsertItem(hItemL item, hList *L) {
    hPosL p = *L, q;
    if (!hCreateNode(&q)) {
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

void hDeleteAtPosition(hPosL pos, hList* L){
    hList p = *L;
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

void hDeleteAll(hList *L){
    while (L != NULL) {
        hDeleteAtPosition(*L, L);
    }
}

hPosL hFirst(hList L) {
    return L;
}

hPosL hLast(hList L){
    hList p=L;
    while (p->next != NULL) {
        p = p->next;
    }
    return p;
}

hPosL hNext(hPosL pos, hList L){
    return pos->next;
}

hPosL hPrevious(hPosL pos, hList L){
    hList p=L;
    if (pos == L){
        return NULL;
    }else {
        while (p->next != pos) {
            p = p->next;
        }
        return p;
    }
}

hItemL hGetItem(hPosL pos, hList L){
    return pos->data;
}

bool hIsEmptyList(hList L) {
    return (L->next == NULL);
}