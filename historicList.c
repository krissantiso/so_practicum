#include "historicList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Private functions

//function that returns the first position of the list
hPosL hFirst(hList L){
    //the first position is the position L points to
    return (L);
}

//function that returns the last position of a given list
hPosL hLast(hList L){
    hList p=L;
    //it searches for the last position by iterating until the positions next is null and returns it
    while (p->next != NULL) {
        p = p->next;
    }
    return p;
}

//function that returns the next position of a given position
hPosL hNext(hPosL pos, hList L){
    //it returns the next position, if its the last it returns LNULL by definition
    return pos->next;
}

//function that returns the previous position of a given position
hPosL hPrevious(hPosL pos, hList L){
    hList p=L;
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

int hGetsizeoflist(hList L){
    int i = 0;
    hPosL p = hFirst(L);
    if (!hIsEmptyList(L)){
        i = 1;
        while (p->next != NULL) {
            p = p->next;
            i++;
        }
    }
    return i;
}

void hPrintList(hList L){
    hPosL pos;
    int i=1;
    if (!hIsEmptyList(L)) {
        pos = hFirst(L);
        while (pos != NULL) {
            printf(" %d: %s\n", i, pos->command);
            i++;
            pos = hNext(pos, L);
        }
    }
    else printf("You didn't execute a command\n");
}

void hPrintNElems(int n, hList L){
    int i;
    int size = hGetsizeoflist(L);  // Size of the list
    hPosL pos = hLast(L);          // Start from the last position

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
        printf(" %d: %s\n", size - i, pos->command);  // Print the command with its position in the list
        pos = hPrevious(pos, L);  // Move to the previous element in the list
    }
}

bool hCreateNode(hPosL* p){
    *p = malloc(sizeof(struct hNode));
    return *p != NULL;
}

void hCreateEmptyList(hList *L){
    *L = NULL;
}

bool hIsEmptyList(hList L){
    return (L == NULL);
}

bool hInsertItem(command newCommand, hList *L){
    hPosL p = *L, q;
    //if the node creating fails it returns false and stops the insertion
    if (!hCreateNode(&q)) {
        return false;
    } else {
        //the item to insert is initialised
        q->command = strdup(newCommand);
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

void hClearList (hList *L){
    hPosL p=*L;
    hPosL next;
    if (hIsEmptyList(*L)) return;
    else {
        while (p != NULL) {
            next = p->next; //saves the next node
            free(p); //deletes the current one
            p=next; //advances
        }
    }
}

command hGetItem (int n, hList L){
    if (hIsEmptyList(L)) return NULL;
    //the data is always stored in the data pointer
    hPosL p = hFirst(L);
    int count;
    if ( n > hGetsizeoflist(L) ) return NULL;
    //real position of the element in the list
    for (count=1; count<n ;count++){
        p=p->next;
    }
    if (hIsEmptyList(L) || count<n) return NULL;
    else return p->command;
}