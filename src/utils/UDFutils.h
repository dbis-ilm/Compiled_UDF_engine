#ifndef UDFUTILS_H
#define UDFUTILS_H
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdbool.h>
#include "../type.h"

#define UDF_MAIN_FUNCTION_NAME "__udf_main"

typedef PyObject* (*Convert_to_PyObject)(char **);
typedef void (*Convert_from_PyObject_and_fill)(char **, PyObject *);

/** A generic structure to maintain the state of UDF executions. */
struct UDFState_t {
    /** Number of UDF input parameters */
    int num_in_cols;
    /** Column indices of input columns */
    int *arg_col_map;
    /** Input parameter types */
    Type *in_types;
    /** UDF result type */
    Type out_type;
    /** Number of processed tuples */
    int processed;
    /** Number of failed tuples */
    int failed;
    /** Input conversion functions, converting to PyObjects */
    Convert_to_PyObject *input_conversion;
    /** Output conversion, converting back from PyObject */
    Convert_from_PyObject_and_fill output_conversion;
};

typedef struct UDFState_t UDFState;

/** Collection of information to dynamically load a UDF from the shared object */
struct UDFInfo_t {
    /** Path to the shared object */
    char *libpath;
    /** UDF function symbol */
    char *function_symbol;
    /** Function symbol of the module initialisation function */
    char *init_func_symbol;
    /** Size of the string buffers for the strings above */
    int buffsize;
};

typedef struct UDFInfo_t UDFInfo;

/** Allocate a UDFInfo object.
 * @return allocated UDFInfo
 */
UDFInfo *alloc_UDFInfo();

/** Free a UDFInfo object.
 * @param info  UDFInfor to be freed.
 */
void free_UDFInfo(UDFInfo *info);

/** Allocate a UDFState object.
 * @param num_in_cols   Number of UDF input parameters
 * @param arg_col_map   Column indices of the input columns
 * @param in_types      Types of the input columns
 * @param out_type      UDF result type
 * @return allocated UDFState
 */
UDFState *UDFState_alloc(int num_in_cols, int *arg_col_map, Type *in_types, Type out_type);

/** Free a UDFState object.
 * @param state     UDFState to be freed
 */
void UDFState_free(UDFState *state);

/** Get the input conversion functions based on the parameter types.
 * @param col_types     UDF parameter types
 * @param num_cols      Number of input parameters
 * @returns array of conversion functions
 */
Convert_to_PyObject *get_input_conversions(Type *col_types, int num_cols);

/** Get the output conversion function based on the return type.
 * @param ret_type  UDF return type
 * @returns output conversion function pointer
 */
Convert_from_PyObject_and_fill get_output_conversion(Type ret_type);

/** Check if UDF function name starts with "vectorized__" prefix
 * @param fname     Function name
 * @return True if vectorized, False otherwise
 */
bool UDF_is_vectorized(char *fname);

/** Compile a UDF with numba by nesting the code into the numba template and running the compilation script.
 * @param UDFFile       File containing the UDF code
 * @param info          UDFInfo to be filled during compilation
 * @param in_types      Types of the input columns
 * @param num_in_cols   Number of UDF input parameters
 * @param out_type      UDF result type
 * @param NRT_necessary Indicate if the numba object mode is necessary (for string support)
 */
void compile_numba_udf(char *UDFFile, char *fname, UDFInfo *info,
    Type *in_types, int num_in_cols, Type out_type, bool NRT_necessary);

/** Compile a UDF using the nuitka compilation script.
 * @param UDFFile       File containing the UDF code
 * @param fname         UDF name in the code file
 * @param info          UDFInfo to be filled during compilation
 */
void compile_nuitka_udf(char *UDFFile, char *fname, UDFInfo *info);

/** Compile a UDF using the cython compilation script.
 * @param UDFFile       File containing the UDF code
 * @param fname         UDF name in the code file
 * @param info          UDFInfo to be filled during compilation
 */
void compile_cython_udf(char* UDFFile, char* fname, UDFInfo *info);

#endif //UDFUTILS_H