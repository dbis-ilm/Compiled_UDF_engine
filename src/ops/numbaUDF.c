#include "numbaUDF.h"
#include <dlfcn.h>
#include "../utils/UDFutils.h"
#include "../utils/fmultiplexer.h"

/* State of the Numba UDF exection */
struct NumbaUDFState_t {
    /** Number of UDF parameters */
    int num_in_cols;
    /** Column offsets of the UDF inputs */
    int *arg_col_map;
    /** Input parameter types */
    Type *in_types;
    /** UDF return type */
    Type out_type;
    /** Number of processed tuples */
    int processed;
    /** Function multiplexer maintaining access to the function pointer */
    FMultiplexer *fmux;
};
typedef struct NumbaUDFState_t NumbaUDFState;

static void load_module(UDFInfo *info, NumbaUDFState *state) {
    fplaceholder func;
    void *pMod;
    pMod = dlopen("/usr/lib/python3.6/config-3.6m-x86_64-linux-gnu/libpython3.6m.so", RTLD_NOW | RTLD_GLOBAL);
    if (!pMod) {
        printf("Error (%s) loading module\n", dlerror());
        assert(0);
    }

    pMod = dlopen(info->libpath, RTLD_LAZY);
    if (!pMod) {
        printf("Error (%s) loading module\n", dlerror());
        assert(0);
    }

    func = (fplaceholder)dlsym(pMod, info->function_symbol);
    if (!func) {
        printf("Error (%s) loading function\n", dlerror());
        assert(0);
        dlclose(pMod);
    }

    state->fmux = fmultiplexer_init(state->in_types, state->num_in_cols, state->out_type, func);
}

static void numbaUDF_close(Operator *op) {
    NumbaUDFState *state = (NumbaUDFState*) op->state;
    fmultiplexer_free(state->fmux);
    free(state->arg_col_map);
    free(state);
    operator_free(op);
}

static int numbaUDF_next(Operator *op) {
    NumbaUDFState *state = (NumbaUDFState*) op->state;
    int i;
    int added_col_idx;

    int n = op->child->next(op->child);
#ifdef PROFILE
    profile_start(op->profile_stats);
#endif
    op->data = op->child->data;

    op->data = reallocate_buffer(op->data, op->child->num_cols, op->num_cols, op->col_types, op->vectorsize);
    added_col_idx = op->num_cols - 1;

    // We can return only after reallocation, otherwise the buffer does not match op->num_cols
    if (n == 0) goto end;

    for (i = 0; i < n; i++) {
        fmultiplexer_run_function_and_fill(state->fmux, op->data, state->arg_col_map,
            state->num_in_cols, i, added_col_idx);
    }
    state->processed += n;
    
end:
#ifdef PROFILE
    profile_stop(op->profile_stats, EXECUTE);
#endif
    return n;
}

Operator *numbaUDF_build(Operator *child, char* UDFFile, char* fname, int num_in_cols, int *arg_col_map,
    Type *in_types, Type out_type) {
    Operator *op;
    NumbaUDFState *state;
    UDFInfo *info;

    // TODO: Check UDFFile of type *.py
    op = operator_alloc(&numbaUDF_next, &numbaUDF_close, child, child->num_cols + 1, child->col_types, "NumbaUDF");

    /* Compile the UDF */
    info = alloc_UDFInfo();
    compile_numba_udf(UDFFile, fname, info, in_types, num_in_cols, out_type, false);
    assert(info->function_symbol);
    // TODO: Proper way out if compilation failed.

    /* UDF adds a column */
    
    op->col_types[op->num_cols - 1] = out_type;
    op->state = malloc(sizeof(NumbaUDFState));
    state = ((NumbaUDFState*)(op->state));
    state->num_in_cols = num_in_cols;
    state->arg_col_map = malloc(num_in_cols * sizeof(int));
    memcpy(state->arg_col_map, arg_col_map, num_in_cols * sizeof(int));
    state->in_types = in_types;
    state->out_type = out_type;
    state->processed = 0;
    state->fmux = NULL;
    load_module(info, state);
    free_UDFInfo(info);

#ifdef PROFILE
    profile_stop(op->profile_stats, BUILD);
#endif
    return op;
}