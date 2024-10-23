#ifndef HELP_H
#define HELP_H

struct command{
    char *name;
    void (*function) (char**);
};

void Cmd_help (char *pcs[]);

#endif //HELP_H
