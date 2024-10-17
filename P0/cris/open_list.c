//
// Created by crist on 24/09/2024.
//
#include "open_list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


tPosLO firstO(tListO L){
    //the first position is the position L points to
    return (L);
}

//function that returns the last position of a given list
tPosLO lastO(tListO L){
    tListO p=L;
    //it searches for the last position by iterating until the positions next is null and returns it
    while (p->next != LNULL) {
        p = p->next;
    }
    return p;
}

//function that returns the next position of a given position
tPosLO nextO(tPosLO pos, tListO L){
    //it returns the next position, if its the last it returns LNULL by definition
    return pos->next;
}

//function that returns the previous position of a given position
tPosLO previousO(tPosLO pos, tListO L){
    tListO p=L;
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



tItemL getItemO (tPosLO p, tListO L){
    return p->file;
}

void createEmptyListO(tListO* L) {
    //initializes a list by making it point to LNULL
    *L = LNULL;
}

bool isEmptyListO(tListO L){
    //initialized lists are empty when they point to null
    return (L == LNULL);
}

void printListO(tListO list) {
    tPosLO pos;
    tItemL item;

    if (!isEmptyListO(list)) {
        pos = firstO(list);
        printf("FileName\tMode\tDescriptor\n");
        while (pos != LNULL) {
            item = getItemO(pos, list);
            printf("%s\t%d\t%d\n", item.name, item.mode, item.des);
            pos = nextO(pos, list);
        }
    }
    else printf("There are no open files\n");
}




bool createNodeO(tPosLO* p){
    *p = malloc(sizeof(struct tNodeO));
    return *p != LNULL;
}

bool insertItemO (tItemL newfile, tListO* L){
    tPosLO p = *L, q;
    //if the node creating fails it returns false and stops the insertion
    if (!createNodeO(&q)) {
        return false;
    } else {
        //the item to insert is initialised
        q->file = newfile;
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

tPosLO findItemO (int d, tListO L){
    tPosLO p;
    for(p=L; (p!= LNULL) && (p->file.des!=d);p=p->next);
    return p;
}

tListO deleteAtPositionO (tPosLO p, tListO *L){
    tPosLO q;
    if (p == *L) {
        *L = (*L) -> next;
    } else if (p->next == LNULL) {//delete lost element
        for (q = *L; q->next != p; q = q->next);//find position in front of p
        q->next = LNULL;
    } else {//delete from the middle
        q = p->next;
        p->file = q->file;
        p->next = q->next;
        p = q; //get rid of the node that used to be right after p
    }
    return p;
    free(p);

}