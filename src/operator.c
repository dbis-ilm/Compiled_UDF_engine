#include "operator.h"

Operator *operator_alloc(nextfunc next, closefunc close, Operator *child, int num_cols,
    Type *col_types, const char *name) {
    Operator *op = malloc(sizeof(Operator));
#ifdef PROFILE
    op->profile_stats = profile_alloc();
    profile_start(op->profile_stats);
#endif
    op->next = next;
    op->close = close;
    op->child = child;
    op->num_cols = num_cols;
    op->name = name;
    if(child) op->vectorsize = child->vectorsize;
    op->col_types = malloc(num_cols * sizeof(Type));
    memcpy(op->col_types, col_types, num_cols * sizeof(Type));
    return op;
}

void operator_free(Operator *op) {
    free(op->col_types);
    free(op);
#ifdef PROFILE
    profile_free(op->profile_stats);
#endif
}