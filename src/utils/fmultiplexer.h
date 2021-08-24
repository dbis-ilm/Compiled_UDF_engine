#ifndef FMULTIPLEXER_H
#define FMULTIPLEXER_H
#include "../type.h"
#include "../buffer.h"
#include "udf_wrappers.h"

struct FMultiplexer_t;

typedef struct FMultiplexer_t FMultiplexer;

/** Initialize a function multiplexer for a given function.
 * @param in_types  Function input types
 * @param num_cols  Number of input parameters
 * @param out_type  Function output type
 * @param func      Pointer to the actual function
 * @return Function multiplexer object
 */
FMultiplexer *fmultiplexer_init(Type *in_types, int num_cols, Type out_type, fplaceholder func);

/** Run the function abstracted by a function multiplexer on a row of the given buffer and write the result back to the buffer.
 * This involves copying the values from the buffer to the generic parameter structure and calling the wrapper function.
 * @param fmux          Function multiplexer
 * @param buf           Data buffer
 * @param arg_col_map   Indexes of argument columns within the buffer
 * @param num_params    Number of function parameters
 * @param row           Row index within the buffer
 * @param out_idx       Index of the result column in the buffer
 */
void fmultiplexer_run_function_and_fill(FMultiplexer *fmux, Buffer buf, int *arg_col_map, int num_params, int row, int out_idx);

/** Free a function multiplexer
 * @param fmux  Function multiplexer to be freed
 */
void fmultiplexer_free(FMultiplexer *fmux);

#endif //FMULTIPLEXER_H