/*
 * TITLE: PROGRAMMING II LABS
 * SUBTITLE: Practical 1
 * AUTHOR 1: --- LOGIN 1: ---
 * AUTHOR 2: --- LOGIN 2: ---
 * GROUP: -
 * DATE: - / - / -
 */

#include "dynamic_list.h"
#include <stdlib.h>
#include "string.h"

/* Write your code here... */

//Generators


bool createNode(tPosL* p){
    *p = malloc(sizeof(struct tNode));
    return *p != LNULL;
}

//function for creating a new empty list
void createEmptyList(tList *L){
    //initializes a list by making it point to LNULL
    *L = LNULL;
}

//function for inserting an item into a list
bool insertItem(tItemL item, tPosL pos, tList *L) {
    tPosL p = *L, q;
    //if the node creating fails it returns false and stops the insertion
    if (!createNode(&q)) {
        return false;
    } else {
        //the item to insert is initialised
        q->data = item;
        q->next = LNULL;
        //if the list is empty
        if (*L == LNULL) {
            //the list now points to the new item thus adding it at the start
            *L = q;
        }
        //if the specified position is LNULL
        else if (pos == LNULL) {
            //it searches for the last position
            while (p->next != LNULL) {
                p = p->next;
            }
            //the last item now points to the new item thus adding it at the end
            p->next = q;
        }
        //if the position is the first one and the list is not empty
        else if (pos == *L){
            //the items next position now points to the first item thus being added before the first item
            q->next = *L;
            //the list now points to the new item thus adding it at the start
            *L = q;
        }
        //if the specified position is any other valid position
        else {
            //it searches for the specified position
            while (p->next != pos) {
                p = p->next;
            }
            //the items next position now points to the item in the given position thus being added before that item
            q->next = p->next;
            //the previous item now points to the new item thus adding it at the given position
            p->next = q;
        }
    }
    return true;

}

//Modifiers

//function for changing the value of an item of a list given its position
void updateItem(tItemL item, tPosL pos, tList* L){
    //it makes the given positions data point to the new data
    pos->data = item;
}

//Destructors

//function for deleting an item of a list given its position
void deleteAtPosition(tPosL pos, tList* L){
    tList p = *L;
    //if the given position is the first
    if (pos == *L) {
        //the list now points the next of the given item thus "skipping" that item
        *L = pos->next;
    //if the given position is the last position
    }else if (pos->next == LNULL){
        //it searches for the last position
        while (p->next != pos) {
            p = p->next;
        }
        //then it makes p next point to LNULL thus stopping pointing to the item at the given position
        p->next = LNULL;
    }else {
        //it searches for the given position
        while (p->next != pos) {
            p = p->next;
        }
        //then it makes p next point to the given positions next thus stopping pointing to the item at the given position
        p->next = pos->next;
    }
    //it frees pos thus completely deleting the item
    free(pos);
}

//Observers

//function for checking if the list is empty
bool isEmptyList(tList L){
    //initialized lists are empty when they point to null
    return (L == LNULL);
}

//function that returns the first position of a given list
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
        //if the position is the first one then it doest have a previous position
        return LNULL;
    }else {
        //it searches for the previous position by iterating until p next is equal to the given position
        while (p->next != pos) {
            p = p->next;
        }
        return p;
    }
}

//function that returns the data of a list in a given position
tItemL getItem(tPosL pos, tList L){
    //the data is always stored in the data pointer
    return pos->data;
}

//function that returns the position of a given item
tPosL findItem(tUserName user, tList L){
    tList p = L;
    //it iterates until the username stored in data is equal to the given user
    while (strcmp(p->data.userName , user) != 0) {
        //if it reaches null then it didnt find it and returns null
        if (p->next == LNULL){
            return LNULL;
        }
        p = p->next;
    }
    return p;
}
