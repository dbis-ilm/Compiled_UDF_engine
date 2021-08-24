#include "buffer.h"

Buffer allocate_buffer(int num_cols, Type *types, int vectorsize) {
    Buffer buf = malloc(num_cols * sizeof(char**));
    int i;

    /* TODO: improve NULL handling. All types other than string will have 0 instead of NULL */
    for (i = 0; i < num_cols; i++) {
        buf[i] = calloc(vectorsize, sizeof_Type(types[i]));
    }
    return buf;
}

void free_buffer(Buffer buf, int num_cols, Type *types, int n) {
    int i, k;
    for (i = 0; i < num_cols; i++) {
        if (types[i] == STRING) {
            for (k = 0; k < n; k++) free(buf[i][k]);
        }
        free(buf[i]);
    }
    free(buf);
}


Buffer reallocate_buffer(Buffer buf, int old_cols, int new_cols, Type *types, int vectorsize) {
    int i;
    Buffer new = realloc(buf, new_cols * sizeof(char**));
    assert(new);
    for (i = old_cols; i < new_cols; i++) {
        new[i] = calloc(vectorsize, sizeof_Type(types[i]));
    }
    return new;
}