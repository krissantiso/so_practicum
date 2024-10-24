// Authors:
// Kris Santiso Sampedro    kris.santiso@udc.es
// Cristina Vazquez Varela  c.vazquezv@udc.es

#ifndef HELP_H
#define HELP_H

struct command{
    char *name;
    void (*function) (char**);
};

void Cmd_help (char *pcs[]);

#endif //HELP_H
