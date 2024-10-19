//
// Created by crist on 07/10/2024.
//

#include "p1.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/utsname.h>
#include <errno.h>
#include <fcntl.h>

void Cmd_cwd (char *pcs[]){
    char dir[MAX];
    if (pcs[0]==NULL) printf("%s\n", getcwd(dir,MAX));
}

void Cmd_makefile (char *pcs[]){
    if (pcs[0]==NULL) {
        printf("The name of the file needs to be provided\n");
        return;
    }
    FILE *file = fopen(pcs[0], "r"); //fopen in mode "r" tries to open the file in read mode
    if (file!=NULL){ //fopen could open the file so that means the file already exists
        printf("Thhis file: %s already exists\n", pcs[0]);
        return;
    }
    file = fopen(pcs[0], "w"); //create the file in writer mode
    if (file==NULL) { //fopen fails
        printf("The file could not be created: %s\n", strerror(errno));
        return;
    }

    printf("%s was created\n", pcs[0]);

    fclose(file); //we just want to create the file
}

void Cmd_listdir (char *tr[]){
    int i, ishid, islong, isacc, islink;
    ishid=islong=isacc=islink=0;
    if (tr[0]==NULL){
        Cmd_cwd(tr);
        return;
    }
    for (i=0; tr[i]!=NULL; i++){
        if (strcmp(tr[i], "-hid")==0) ishid=1;
        else if (strcmp(tr[i], "-long")==0) islong=1;
        else if (strcmp(tr[i], "-link")==0) islink=1;
        else if (strcmp(tr[i], "-acc")==0) isacc=1;
        else break;
    }

    for (;tr[i]!=NULL;i++) {
        //if(Cmd_listdir(tr[i], ishid, islong, islink, isacc) == -1)
            printf("Cannot list % s: %s\n", tr[i], strerror(errno));
    }
}


