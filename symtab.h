#ifndef SYMTAB_H
#define SYMTAB_H


typedef struct stnode Stn_t;

struct stnode {
    char *s;
    int v;
    Stn_t *n;
};

typedef struct symtab St_t;

struct symtab {
    Stn_t *top;
    Stn_t *curr;
};

/* Symbol Table */
static St_t *st;

int pull_symbol_table(char *);
void push_symbol_table(char *, int);
void init_symbol_table();


#endif 