#ifndef CYTHONUDF_H
#define CYTHONUDF_H

#include "../operator.h"
#include <stdbool.h>

/** Build a cython UDF operator on top of an existing query tree. The operator adds a column to the intermediate results
 * containing the results of the UDF computation.
 * @param child         Child operator producing the input buffers
 * @param UDFFile       Path to the file containing the UDF code
 * @param fname         Name of the UDF within the UDFFile
 * @param num_in_cols   Number of input columns to the UDF
 * @param arg_col_map   Column offsets of the UDF inputs
 * @param in_types      UDF input parameter types
 * @param out_type      UDF return type, type of the added result column
 * @return Pointer to the operator
 */
Operator *cythonUDF_build(Operator *child, char* UDFFile, char* fname, int num_in_cols, int *arg_col_map,
    Type *in_types, Type out_type);

#endif //CYTHONUDF_H