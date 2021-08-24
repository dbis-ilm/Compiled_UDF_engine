#ifndef OPERATOR_H
#define OPERATOR_H

#include "buffer.h"
#ifdef PROFILE
#include "profile.h"
#endif

typedef struct Operator_t Operator;

typedef int (*nextfunc)(Operator*);
typedef void (*closefunc)(Operator*);

/** Basic operator structure with elements shared by all operators. 
 *  Each operator must implement a next and a close function.
 *  Follows volcano iterator model in a vectorized fashion. */
struct Operator_t {
    /** Produces a vector of results in data */
    nextfunc next;
    /** Frees the operator memory, including all built structures. */
    closefunc close;
    /** Child operator. Might be NULL in case of scans. We only have single-child operators here. */
    Operator* child;
    /** Number of columns of the result buffer */
    int num_cols;
    /** Column types of the result buffer */
    Type *col_types;
    /** Operator name */
    const char *name;
    /** Number of result rows per buffer */
    int vectorsize;
    /** Generic pointer to an operator state if necessary */
    void *state;
    /** The data buffer, filled by a next call */
    Buffer data;
    #ifdef PROFILE
    /** Profile information of operator execution */
    ProfileStats *profile_stats;
    #endif
};

/** Allocate an operator.
 * @param next      next function
 * @param close     close function
 * @param child     child operator if present
 * @param num_cols  number of columns of the operator's result
 * @param col_types result types
 * @param name      operator name
 * @return allocated operator
 */
Operator *operator_alloc(nextfunc next, closefunc close, Operator *child, int num_cols,
    Type *col_types, const char *name);

/** Free an operator 
 * @param op    Operator to be freed
 */
void operator_free(Operator *op);

/* Include operator headers here AFTER the operator struct definition. */
#include "ops/scan.h"
#include "ops/pythonUDF.h"
#include "ops/cythonUDF.h"
#include "ops/numbaUDF.h"
#include "ops/numbaNRTUDF.h"
#include "ops/nuitkaUDF.h"

#endif //OPERATOR_H