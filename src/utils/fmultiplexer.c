#include <stdlib.h>
#include <math.h>
#include "fmultiplexer.h"

/** Abstract from different function signatures and provide transparent execution. */
struct FMultiplexer_t {
    /** Unique ID of the function signature, being a linear combination of the types */
    FSigID sigID;
    /** Pointer to the actual function. Is set once during initialization and casted for execution. */
    fplaceholder thefunc;
    /** Function that wraps the actual function. Performs input casting, function casting and output casting */
    WrapperFunc wrapper;
    /** Maximum number of parameters that can be handled */
    int max_params;
    /** Pre-allocated memory for function parameters, so allocation takes place only once. */
    char ***allocated_params;
};

/** Compute the FSigID, a unique value for a given function signature. 
 *  The value is a linear combination of the signature types
 *  (basically encoded in Z_base, base = 4 currently and converted to decimal), i.e.
 *  sig = out_type * base^0 + in_type[0] * base^1 + in_type[1] * base^2 + ...
 * 
 *  Example: char* function(double, int) is encoded as:
 *  3 * 4^0 + 2 * 4^1 + 1 * 4^2 = 123 to the base of 4 = 27 to the base of 10
 * 
 * @param in_types  Function input types
 * @param num_cols  Number of input parameters
 * @param out_type  Function output type
 * @return Unique function signature ID
 */
static FSigID compute_FSigID(Type *in_types, int num_cols, Type out_type) {
    long base = kmaxTypeValue;
    FSigID sig = (long) out_type;
    long i;
    base++;
    for (i = 0; i < num_cols; i++) {
        sig += ((long) in_types[i]) * pow(base, i + 1);
    }
    return sig;
}

FMultiplexer *fmultiplexer_init(Type *in_types, int num_cols, Type out_type, fplaceholder func) {
    int i;
    FMultiplexer *fmux = malloc(sizeof(FMultiplexer));
    fmux->sigID = compute_FSigID(in_types, num_cols, out_type);
    fmux->thefunc = func;
    fmux->max_params = 2;
    /* Intentionally keep the first element NULL to adress over num_params */
    fmux->allocated_params = malloc((fmux->max_params + 1) * sizeof(char**));
    for (i = 1; i < fmux->max_params + 1; i++) {
        fmux->allocated_params[i] = malloc(i * sizeof(char*));
    }
    fmux->wrapper = fmux_wrapper(fmux->sigID);
    return fmux;
}

void fmultiplexer_run_function_and_fill(FMultiplexer *fmux, Buffer buf, int *arg_col_map, int num_params, int row, int out_idx) {
    int i;
    for (i = 0; i < num_params; i++) {
        fmux->allocated_params[num_params][i] = buf[arg_col_map[i]][row];
    }
    fmux->wrapper(fmux->thefunc, fmux->allocated_params[num_params], &(buf[out_idx][row]));
}

void fmultiplexer_free(FMultiplexer *fmux) {
    int i;
    for (i = 1; i < fmux->max_params + 1; i++) {
        free(fmux->allocated_params[i]);
    }
    free(fmux->allocated_params);
    free(fmux);
}