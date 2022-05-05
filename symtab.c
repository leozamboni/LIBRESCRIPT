#include "symtab.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_symbol_table() {
    st = malloc(sizeof(St_t));
    st->top = st->curr = NULL;        
}

_Bool in_symbol_table(Stn_t *top, char *s) {
    if (!top) return 0;

    if (strcmp(s, top->s) == 0) return 1;

    return in_symbol_table(top->n, s);
}

void push_symbol_table(char *s, int v) {
    if (in_symbol_table(st->top, s)) {
        error(s);
    }
    printf("str: %s | value: %d\n", s, v);
    Stn_t *new = malloc(sizeof(Stn_t));
    new->s = malloc(sizeof(char) * strlen(s));
    strcpy(new->s, s);
    new->v = v;
    new->n = NULL;
    if (st->curr) st->curr->n = new;
    if (!st->top) st->top = new;
    st->curr = new;
}

int check_symbol_table(Stn_t *top, char *s) {
    if (!top) return 0;

    if (strcmp(s, top->s) == 0) return top->v;

    return check_symbol_table(top->n, s);
}

int pull_symbol_table(char *s) {
    return check_symbol_table(st->top, s);
}