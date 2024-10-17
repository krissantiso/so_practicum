#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/utsname.h>
#include <errno.h>
#include <fcntl.h>
#define MAX 2048
#define LNULL NULL


//COMMAND LIST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// DATA TYPES

typedef char tCommand[MAX];

typedef struct tNode* tPosL;

struct tNode {
    tCommand command;
    tPosL next;
};

typedef tPosL tList;

tList L;

//FUNCTIONS

//Generators
bool createNode(tPosL* p){
    *p=malloc(sizeof(struct tNode));
    return *p!=LNULL;
}

void createEmptyList(tList* L) {
    *L=LNULL;
}

void insertItem(tCommand cmd, tList* L) {
    tPosL q,p=*L;
    if (!createNode(&q)) {
        return;
    } else {
        strcpy(q->command,cmd);
        q->next=LNULL;
        if (*L==LNULL) {
            *L=q;
        } else {
            while(p->next!=LNULL) {
                p=p->next;
            }
            p->next=q;
        }
    }
}

//Destructors
void deleteAtPosition(tPosL pos, tList* L) {
    if (pos==*L) {
        *L=pos->next;
    } else if (pos->next==LNULL) {
        tPosL p;
        p=*L;
        while (p->next!=pos) {
            p=p->next;
        }
        p->next=LNULL;
    } else {
        tPosL p;
        p=*L;
        while (p->next!=pos) {
            p=p->next;
        }
        p->next=pos->next;
    }
    free(pos);
}

void deleteLastPos(tList* L) {  //THIS CAN PROBALY BE OPTIMIZED
    tPosL p = *L;
    if (L==LNULL) {
        return;
    } else if (p->next == LNULL) {
        L = LNULL;
        free(p);
    } else {
        tPosL q = p -> next;
        while (q!=LNULL) {
            p=p->next;
            q=q->next;
        }
        p->next = LNULL;
        free(q);
    }
}

bool deleteAll(tList* L) {
    if (*L==NULL) {
        return false;
    } else {
        tPosL p, q;
        p = *L;
        q = p->next;
        while (p != NULL) {
            free(p);
            p=q;
            if (q!=NULL) {
                q=q->next;
            }
        }
        *L = NULL;
        return true;
    }
}

//Observers
bool isEmptyList(tList L) {
    return (L==LNULL);
}

tPosL last(tList L) {
    tPosL p;
    p=L;
    while(p->next!=LNULL) {
        p=p->next;
    }
    return p;
}

tCommand *getItem(tPosL pos, tList L) {
    return &pos->command;
}

tPosL findCommand(int n, tList L) {
    tPosL p;
    p=L;
    int i = 0;
    while (i != n && p!=LNULL) {
        p = p->next;
        i++;
    }
    return p;
}

void printListN(int N, tList L) {   //VERSION THAT DOESNT PRINT HISTORIC ITSELF
    int i = 0;
    int c;
    tPosL p = L;
    while (p->next!=LNULL) {
        i++;
        p = p->next;
    }
    if ((i-N)+1<=0) {
        printf("Not enough commands to print\n");
    } else {
        tPosL q = findCommand((i-N), L);
        c = i-N;
        while (q->next!=NULL) {
            printf("%d -> %s",c, q->command);
            q = q->next;
            c++;
        }
    }
}

void printListFull(tList L) {   //VERSION THAT DOESNT PRINT HISTORIC ITSELF
    tPosL p = L;
    int i = 0;
    if (p->next == NULL) {
        printf("Command history is empty\n");
    } else {
        while (p->next != LNULL) {
            printf("%d -> %s", i, p->command);
            p = p->next;
            i++;
        }
    }
}

//FILE LIST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

typedef char tMode[4];      //TE CAMBIE LOS NOMBRES PARA QUE DESPUES DEL T VAYAN EN MAYUSCULA PARA QUE SEAN COMO LOS DE
typedef int tDescriptor;    //LA OTRA LISTA
typedef char tName[MAX/2];

typedef struct tFile* tPosF;

struct tFile {
    tMode mode;
    tDescriptor df;
    tName name;
    tPosF next;
};

typedef tPosF tFList;

tFList F;

//FUNCTIONS

//Generators
bool createNodeFile(tPosF* p){
    *p=malloc(sizeof(struct tFile));
    return *p!=LNULL;
}

void createEmptyFList(tFList* L) {
    *L=LNULL;
}

void AddFile(tDescriptor df, tName name, tMode mode, tFList* L) {   //LO MISMO QUE CON EL OTRO INSERT
    tPosF q,p=*L;
    if (!createNodeFile(&q)) {
        return;
    } else {
        strcpy(q->name,name);
        strcpy(q->mode, mode);
        q->df = df;
        q->next=LNULL;
        if (*L==LNULL) {
            *L=q;
        } else {
            while(p->next!=LNULL) {
                p=p->next;
            }
            p->next=q;
        }
    }
}

//Destructors
/* bool deleteFile(tPosF pos, tFList* L){
    if (pos==NULL) return false;
    else {
        if (pos==*L) *L=pos->next;
        else {
        tPosF p=*L;
        if (pos->next==LNULL) {
                while (p->next!=pos) {
                    p=p->next;
                }
                p->next=LNULL;
        } else {
            while (p->next!=pos) {
                p=p->next;
            }
            p->next=pos->next;
        }
    free(pos);
    while (p->next!=NULL){
        p->df=p->df-1;
        p=p->next;
    } 
    }
    return true;
    }
} */    //VERSION 1 DEL DELETE FILE

bool deleteFile(tPosF pos, tFList* L){  //VERSION 2 DEL DELETE FILE DESPUES DE QUE LE PREGUNTASE QUE COJONES
    if (pos==NULL) return false;
    else if (pos==*L){ *L=pos->next; return true;}
    else {
        tPosF p=*L;
        if (pos->next==LNULL) {
            while (p->next!=pos) {
                p=p->next;}
            p->next=LNULL;
        } else {
            while (p->next!=pos) {
                p=p->next;}
            p->next=pos->next;
        }
        free(pos);
        while (p->next!=NULL){
            p->df=p->df-1;
            p=p->next;}
        return true;
    }
}

//Observers
tDescriptor getDf(tPosF pos, tFList L){
    return pos->df;
}

tName* getName(tPosF pos, tFList L){
    return &pos->name;
}

tMode* getMode(tPosF pos, tFList L){
    return &pos->mode;
}

/* tPosF FindFile(int df, tFList L) {  //NOTA PARA MI: FUNCIONA DISTINTO QUE FINDCMD Y QUE FIND ITEM
    tPosF p;
    p=L;
    int i=0;
    while (i!=df) {
        if (p->next==LNULL) {
            return LNULL;
        }
        p=p->next;
        i++;
    }
    
    return p;
} */    //VERSION 1 DEL FINDFILE

tPosF FindFile(int df, tFList L) {  //VERSION 2 DEL FIND FILE
    tPosF p;
    p=L;
    int i=3;
    while (i!=df) {
        if (p->next==LNULL) {
            return LNULL;
        }
        p=p->next;
        i++;
    }
    return p;
}

void ListOpenFiles (tFList L) { //NOTA PARA MI: NO SE COMO FUNCIONA
    if (L == NULL) {
        printf("Cannot list open files\n");
    } else {
        tPosF p = L;
        while (p != NULL) {
            printf("directory: %d->%s - %s\n", p->df, p->name, p->mode);
            p = p->next;
        }
    }
}

//END OF LIST STUFF!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//COMMAND FUNCTIONS //ESTAN EN ORDEN DEL PDF PORQUE ME ES MAS COMODO

struct CMD {    //COPIADO DEL MIO PORQUE ES MEJOR
    char *name;
    void (*function) (char**);
};

void DoCommand (char *pz[]);    //COPIADO DEL MIO

int BreakLine (char* lin, char* pz[]);  //COPIADO DEL MIO

void Cmd_authors(char *pz[]) {  //COPIADO DEL MIO (PORQUE ES MEJOR) :D
    if (pz[0] == NULL) {
        printf("Diego Arturo Contreras Gonzalez\tdiego.contreras@udc.es\n"
               "Antia Vazquez Marino\t\tantia.vazquez@udc.es\n");
    } else if (strcmp(pz[0], "-n") == 0) {
        printf("Diego Arturo Contreras Gonzalez\nAntia Vazquez Marino\n");
    } else if (strcmp(pz[0], "-l") == 0) {
        printf("diego.contreras@udc.es\nantia.vazquez@udc.es\n");
    } else {
        printf("Invalid argument\n");
    }
}

void Cmd_pid(char *tr[]){   //FUNCIONA BIEN NO TOCAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
printf("Pid del proceso %d\n", getpid());
}

void Cmd_ppid (char *tr[]){ //FUNCIONA BIEN NO TOCAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
printf("Pid del proceso del padre %d\n", getppid());
}

void Cmd_chdir (char *tr[]){    //FUNCIONA BIEN NO TOCAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	char diractual[MAX];
	if (tr[0]==NULL){
		printf("%s\n",getcwd(diractual,MAX));
    }
	else{
		if (chdir(tr[0])==-1){
			printf("Impossible change of dir"); 
        }
    }
}

void Cmd_date(char *pz[]){  //FUNCIONA BIEN NO TOCAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    time_t t = time(NULL);
    struct tm tm= *localtime(&t);

    if (pz[0]==NULL){
        printf("%02d/%02d/%04d\n", tm.tm_mday,(tm.tm_mon+1),(tm.tm_year+1900));
        printf("%02d:%02d:%02d\n",tm.tm_hour,tm.tm_min,tm.tm_sec);
    } else if (strcmp(pz[0],"-d")==0){
        printf("%02d/%02d/%04d\n", tm.tm_mday,(tm.tm_mon+1),(tm.tm_year+1900));
    } else if (strcmp(pz[0],"-t")==0){
        printf("%02d:%02d:%02d\n",tm.tm_hour,tm.tm_min,tm.tm_sec);
    } else printf("Incorrect argument\n");
}

void Cmd_historic(char *pz[]) { //COPIADO DEL MIO
    int n;
    char *endptr;
    char *pcs [MAX/2];
    char line [MAX];
    tPosL p;
    if (pz[0]==NULL) {
        printListFull(L);
    } else if (*pz[0]=='-') {
        n = strtol(pz[0]+1, &endptr, 10); //(*pz[0])[1]
        if (*endptr == '\0' && endptr != pz[0]+1) {
            printListN(n, L);   //WE NEED TO SPECIFY IN HELP THAT HISTORIC STARTS ENUMERATING AT 0
        }
    } else {
        n = strtol(pz[0], &endptr,10);
        if (*endptr == '\0' && endptr != pz[0]) {
            deleteAtPosition(last(L),&L);
            p = findCommand(n,L);
            if (p == LNULL) {
                printf("Command number %d not found\n",n);
            } else {
                strcpy(line, findCommand(n, L)->command);
                BreakLine(line,pcs);
                DoCommand(pcs);
            }
        } else {
            printf("Invalid argument\n");
        }
    }
}

void Cmd_open (char * tr[]) {   //AQUI ES DONDE TE TIENES QUE PONER A MIRAR
    int i,df, mode=0;
    if (tr[0]==NULL) {
        ListOpenFiles(F);
        return;
    }
    for (i=1; tr[i]!=NULL; i++) {
        if (!strcmp(tr[i], "cr")) mode |= O_CREAT;
        else if (!strcmp(tr[i], "ex")) mode |= O_EXCL;
        else if (!strcmp(tr[i], "ro")) mode |= O_RDONLY;
        else if (!strcmp(tr[i], "wo")) mode |= O_WRONLY;
        else if (!strcmp(tr[i], "rw")) mode |= O_RDWR;
        else if (!strcmp(tr[i], "ap")) mode |= O_APPEND;
        else if (!strcmp(tr[i], "tr")) mode |= O_TRUNC;
        else break;
    }
    if ((df=open(tr[0],mode,0777))==-1) {
        perror("Imposible abrir fichero");
    } else {
        AddFile(df,tr[0],tr[1],&F);
        printf ("Entry %d ahs been added to the file list",df);
    }
}

void Cmd_close (char *tr[]) {   //AQUI ES DONDE TE TIENES QUE PONER A MIRAR
    int df;
    if (tr[0]==NULL || (df=atoi(tr[0]))<0) {
        ListOpenFiles(F);
        return;
    }
    if (close(df)==-1) {
        perror("Cannot close descriptor\n");
    } else {
        if (deleteFile(FindFile(df,F),&F)==false) {
            printf("Cannot delete file\n");
        } else {
            printf("File deleted\n");
        }
    }
}

void Cmd_dup (char * pz[]) {    //AQUI ES DONDE TE TIENES QUE PONER A MIRAR
    int df, duplicado;
    char aux[MAX],*p;
    if (pz[0] == NULL || (df=atoi(pz[0]))<0) {
        ListOpenFiles(F);
        return;
    }
    duplicado=dup(df);
    p=*getName(FindFile(df,F),F);
    sprintf (aux,"dup %d (%s)",df, p);
    AddFile(duplicado,aux,p,&F);
    fcntl(duplicado,F_GETFL);
}

void Cmd_infosys (char *pz[]) { //FUNCIONA BIEN NO TOCAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    struct utsname unameData;
    uname(&unameData);
    printf("%s\n%s\n%s\n%s\n%s\n",unameData.sysname,unameData.version,unameData.release,unameData.machine,unameData.nodename);
}

void Cmd_quit(char *pz[]) { //COPIADO DEL MIO PORQUE ES MEJOR
    if (pz[0]==NULL) {
        exit(0);
        return;
    }
    printf("Invalid argument\n");
}

//HELP FUNCTIONS //COPIADO DEL MIO

void helpAuthors (char *pz[]) {
    printf("authors: Prints the names and logins of the program authors. authors -l prints\n"
           "only the logins and authors -n prints only the names\n");
}

void helpPid (char *pz[]) {
    printf("pid: Prints the pid of the process executing the shell.\n");
}

void helpPpid (char *pz[]) {
    printf("ppid: Prints the the pid of the shell's parent process.\n");
}

void helpCd (char *pz[]) {
    printf("cd [dir]: Changes the current working directory of the shell to dir When\n"
           "invoked without arguments it prints the current working directory.\n");
}

void helpDate (char *pz[]) {
    printf("date [-t|-d]: Prints the current date in the format DD/MM/YYYY and the current\n"
           "time in the format hh:mm:ss.\n\n"
           "\tdate -d: Prints the current date in the format DD/MM/YYYY\n\n"
           "\tdate -t: Prints and the current time in the format hh:mm:ss.\n");
}

void helpHistoric (char *pz[]) {
    printf("historic [N|-N]: Shows the historic of commands executed by this shell. In order to\n"
           "do this, a list to store all the commands input to the shell must be\n"
           "implemented.\n"
           "\t- historic: Prints all the commands that have been input with their\n"
           "order number\n"
           "\t- historic N: Repeats command number N (from historic list)\n"
           "\t- historic -N: Prints only the lastN commands.\n");
}

void helpOpen (char *pz[]) {
    printf("open [file] mode: Opens a file and adds it (together with the file descriptor and the\n"
           "opening mode to the list of shell open files. For the mode we’ll use cr for\n"
           "O CREAT, ap for O APPEND, ex for O EXCL, ro for O RDONLY,\n"
           "rw for O RDWR, wo for O WRONLY and tr for O TRUNC.\n"
           "Open without arguments lists the shell open files. For each file it lists its\n"
           "descriptor, the file name and the opening mode.\n");
}

void helpClose (char *pz[]) {
    printf("close [df]: Closes the df file descriptor and eliminates the corresponding item from\n"
           "the list.\n");
}

void helpDup (char *pz[]) {
    printf("dup [df]: Duplicates the df file descriptor.\n");
}

void helpInfo (char *pz[]) {
    printf("infosys: Prints information on the machine running the shell.\n");
}

void helpHelp (char *pz[]) {
    printf("help [cmd]: help displays a list of available commands. help cmd gives a brief help\n"
           "on the usage of command cmd.\n");
}

void helpQuit (char *pz[]) {
    printf("quit: Ends the shell.\n");
}

void helpExit (char *pz[]) {
    printf("exit: Ends the shell.\n");
}

void helpBye (char *pz[]) {
    printf("bye: Ends the shell.\n");
}

struct CMD helpOPTS[] = {
        {"authors", helpAuthors},
        {"pid", helpPid},
        {"ppid", helpPpid},
        {"cd",helpCd},
        {"date",helpDate},
        {"historic", helpHistoric},
        {"open",helpOpen},
        {"close",helpClose},
        {"dup", helpDup},
        {"info",helpInfo},
        {"help",helpHelp},
        {"quit", helpQuit},
        {"exit", helpExit},
        {"bye", helpBye},
        {NULL, NULL},
};

void Cmd_help (char *pz[]) {
    int i;
    if (pz[0]==NULL) {
        printf("authors: Prints the names and logins of the program authors. authors -l prints\n"
               "only the logins and authors -n prints only the names\n\n"
               "pid: Prints the pid of the process executing the shell.\n\n"
               "ppid: Prints the pid of the shell's parent process.\n\n"
               "cd [dir]: Changes the current working directory of the shell to dir When\n"
               "invoked without arguments it prints the current working directory.\n\n"
               "date [-t|-d]: Prints the current date in the format DD/MM/YYYY and the current\n"
               "time in the format hh:mm:ss.\n\n"
               "\tdate -d: Prints the current date in the format DD/MM/YYYY\n\n"
               "\tdate -t: Prints and the current time in the format hh:mm:ss.\n\n"
               "historic [N|-N]: Shows the historic of commands executed by this shell. In order to\n"
               "do this, a list to store all the commands input to the shell must be\n"
               "implemented.\n"
               "\t- historic: Prints all the commands that have been input with their\n"
               "order number\n"
               "\t- historic N: Repeats command number N (from historic list)\n"
               "\t- historic -N: Prints only the lastN commands.\n\n"
               "open [file] mode: Opens a file and adds it (together with the file descriptor and the\n"
               "opening mode to the list of shell open files. For the mode we’ll use cr for\n"
               "O CREAT, ap for O APPEND, ex for O EXCL, ro for O RDONLY,\n"
               "rw for O RDWR, wo for O WRONLY and tr for O TRUNC.\n"
               "Open without arguments lists the shell open files. For each file it lists its\n"
               "descriptor, the file name and the opening mode.\n\n"
               "close [df]: Closes the df file descriptor and eliminates the corresponding item from\n"
               "the list.\n\n"
               "dup [df]: Duplicates the df file descriptor.\n\n"
               "infosys: Prints information on the machine running the shell.\n\n"
               "help [cmd]: help displays a list of available commands. help cmd gives a brief help\n"
               "on the usage of command cmd.\n\n"
               "quit: Ends the shell.\n\n"
               "exit: Ends the shell.\n\n"
               "bye: Ends the shell.\n");
        return;
    }
    for (i=0;helpOPTS[i].name!=NULL; i++)
        if (!strcmp(pz[0],helpOPTS[i].name)) {
            (*helpOPTS[i].function)(pz);
            return;
        }
    printf ("Invalid argument\n");
}

struct CMD C[]={
        {"authors",Cmd_authors},
        {"pid",Cmd_pid},
        {"ppid",Cmd_ppid},
        {"chdir",Cmd_chdir},
        {"date",Cmd_date},
        {"historic",Cmd_historic},
        {"open",Cmd_open},
        {"close",Cmd_close},
        {"dup",Cmd_dup},
        {"infosys",Cmd_infosys},
        {"help", Cmd_help},
        {"quit",Cmd_quit},
        {"exit",Cmd_quit},
        {"bye",Cmd_quit},
        {NULL,NULL},
};

void DoCommand (char *pz[]) { //COPIADO DEL MIO PORQUE ES MEJOR
    int i;
    if (pz[0]==NULL)
        return;
    for (i=0;C[i].name!=NULL; i++)
        if (!strcmp(pz[0],C[i].name)){
            (*C[i].function)(pz+1);
            return;
        }
    printf ("Command %s not found\n",pz[0]);
}

int BreakLine (char *lin, char *pz[]) { //COPIADO DEL MIO PORQUE ES MEJOR
    if ((pz[0]=strtok(lin," \n\t"))==NULL)
        return 0;
    int i=1;
    while ((pz[i]=strtok(NULL," \n\t"))!=NULL)
        i++;
    return i;
}

int main(int argc, char *argv[]) {  //COPIADO DEL MIO PORQUE ES MEJOR
    char line [MAX];
    char *pcs [MAX/2];
    createEmptyList(&L);
    createEmptyFList(&F);
    while (1) {
        printf("#)");
        fgets(line, MAX, stdin);
        insertItem(line, &L);
        if (BreakLine(line, pcs) == 0)
            continue;
        DoCommand(pcs);
    }
}