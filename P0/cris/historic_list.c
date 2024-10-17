//
// Created by crist on 18/09/2024.
//
#include "historic_list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


tPosL first(tList L){
    //the first position is the position L points to
    return (L);
}

//function that returns the last position of a given list
tPosL last(tList L){
    tList p=L;
    //it searches for the last position by iterating until the positions next is null and returns it
    while (p->next != LNULL) {
        p = p->next;
    }
    return p;
}

//function that returns the next position of a given position
tPosL next(tPosL pos, tList L){
    //it returns the next position, if its the last it returns LNULL by definition
    return pos->next;
}

//function that returns the previous position of a given position
tPosL previous(tPosL pos, tList L){
    tList p=L;
    if (pos == L){
        //if the position is the first one then it doesnt have a previous position
        return LNULL;
    }else {
        //it searches for the previous position by iterating until p next is equal to the given position
        while (p->next != pos) {
            p = p->next;
        }
        return p;
    }
}

int getsizeoflist(tList L){
    int i = 0;
    tPosL p = first(L);
    if (!isEmptyList(L)){
        i = 1;
        while (p->next != NULL) {
            p = p->next;
            i++;
        }
    }
    return i;
}


char * getItem (int n, tList L){
    //the data is always stored in the data pointer
    tPosL p= first(L);
    int count;
    if (n>getsizeoflist(L)) return NULL;
     //real position of the element in the list
    for (count=1; count<n ;count++){
        p=p->next;
    }
    if (isEmptyList(L) || count<n) return NULL;
    else return p->cmd;
}

void createEmptyList(tList* L) {
    //initializes a list by making it point to LNULL
    *L = LNULL;
}

bool isEmptyList(tList L){
    //initialized lists are empty when they point to null
    return (L == LNULL);
}

void printList(tList list) {
    tPosL pos;
    int i=1;
    if (!isEmptyList(list)) {
        pos = first(list);
        while (pos != LNULL) {
            printf(" %d: %s\n", i, pos->cmd);
            i++;
            pos = next(pos, list);
        }
    }
    else printf("You didn't execute a command\n");
}

void printNElems(tList L, int n){
    int i;
    int size = getsizeoflist(L);  // Size of the list
    tPosL pos = last(L);          // Start from the last position

    if (size == 0) {
        printf("There are no commands yet\n");
        return;
    }

    if (n > size) {
        printf("You have to insert a valid number\n");
        return;
    }

    // Print the last n commands
    for (i = 0; i < n; i++) {
        printf(" %d: %s\n", size - i, pos->cmd);  // Print the command with its position in the list
        pos = previous(pos, L);  // Move to the previous element in the list
    }
}


bool createNode(tPosL* p){
    *p = malloc(sizeof(struct tNode));
    return *p != LNULL;
}

bool insertItem (char * newcommand, tList* L){
    tPosL p = *L, q;
    //if the node creating fails it returns false and stops the insertion
    if (!createNode(&q)) {
        return false;
    } else {
        //the item to insert is initialised
        q->cmd = strdup(newcommand);
        q->next = LNULL;
        //if the list is empty
        if (*L == LNULL) {
            //the list now points to the new item thus adding it at the start
            *L = q;
        }

        else {
            //it searches for the last position
            while (p->next != LNULL) {
                p = p->next;
            }
            //the last item now points to the new item thus adding it at the end
            p->next = q;
        }


    }
    return true;
}

void clearList (tList *L){
    tPosL p=*L;
    if (isEmptyList(*L)) return;
    else {
        while (p->next->next != LNULL) {
            p = p->next;
        }
        for (p=p->next;previous(p, *L)!=LNULL; p=previous(p, *L)){
            p->next=LNULL;
        }
    }
    free(p);
}


