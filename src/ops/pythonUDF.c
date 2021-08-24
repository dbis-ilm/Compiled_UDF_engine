#include "pythonUDF.h"
#include <stdbool.h>
#include <libgen.h> 

/** State of the PythonUDF execution */
struct PythonUDFState_t {
    /** Common state properties */
    UDFState *common;
    /** Pointer to the UDF module object */
    PyObject *pModule;
    /** Pointer to the UDF object */
    PyObject *pFunc;
};

typedef struct PythonUDFState_t PythonUDFState;

static void pythonUDF_close(Operator *op) {
    PythonUDFState *state = (PythonUDFState*) op->state;
    Py_DECREF(state->pFunc);
    Py_DECREF(state->pModule);
    printf("%i out of %i UDF calls failed.\n", state->common->failed, state->common->processed);
    UDFState_free(state->common);
    free(state);
    operator_free(op);
}

static int pythonUDF_next(Operator *op) {
    PythonUDFState *state = (PythonUDFState*) op->state;
    int i, k;
    int added_col_idx;
    PyObject *pValue, *pRet = NULL;
    PyObject *pArgs = PyTuple_New(state->common->num_in_cols);    

    int n = op->child->next(op->child);
#ifdef PROFILE
    profile_start(op->profile_stats);
#endif
    op->data = op->child->data;

    op->data = reallocate_buffer(op->data, op->child->num_cols, op->num_cols, op->col_types, op->vectorsize);
    added_col_idx = op->num_cols - 1;

    // We can return only after reallocation, otherwise the buffer does not match op->num_cols
    if (n == 0) goto end;

    for (k = 0; k < state->common->num_in_cols; k++) {
        int arg_col_idx = state->common->arg_col_map[k];
        PyObject *list = PyList_New(n);
        for (i = 0; i < n; i++) {
            char *elem = op->data[arg_col_idx][i];
            pValue = Py_None;
            if (elem) {  
                pValue = state->common->input_conversion[k](&elem);
                if (!pValue) {
                    fprintf(stderr, "Cannot convert argument for row %i.\n", state->common->processed + i);
                    state->common->failed++;
                    pValue = Py_None;
                }
            }    
            PyList_SetItem(list, i, pValue);
        }
        PyTuple_SetItem(pArgs, k, list);
    }

    /* Run */
    pRet = PyObject_CallObject(state->pFunc, pArgs);   

    /* Get returns */
    if (pRet != NULL) {
        for (i = 0; i < n; i++) {
            pValue = PyList_GetItem(pRet, i);
            if (pValue != Py_None) {
                state->common->output_conversion(&(op->data[added_col_idx][i]), pValue);
                //Py_DECREF(pValue);
            }
        }
    }
    else {
        fprintf(stderr,"UDF Call failed\n");
        PyErr_Print();
        state->common->failed+=n;
    }
    state->common->processed += n;
end:
    Py_XDECREF(pArgs);
    Py_XDECREF(pRet);
#ifdef PROFILE
    profile_stop(op->profile_stats, EXECUTE);
#endif
    return n;
}

Operator *pythonUDF_build(Operator *child, char* UDFFile, char* fname, int num_in_cols, int *arg_col_map,
    Type *in_types, Type out_type) {
    Operator *op;
    PythonUDFState *state;
    PyObject *pName;
    char *UDFFile_basename = basename(UDFFile);
    char *UDFFile_dirname = dirname(UDFFile);
    char buf[1024];
    bool is_vectorized = UDF_is_vectorized(fname);
    nextfunc next = &pythonUDF_next;
    char *opname = is_vectorized ? "PythonUDF(vec)" : "PythonUDF";

    /* UDF adds a column */
    op = operator_alloc(next, &pythonUDF_close, child, child->num_cols + 1, child->col_types, opname);
    op->col_types[op->num_cols - 1] = out_type;

    op->state = malloc(sizeof(PythonUDFState));
    state = ((PythonUDFState*)(op->state));
    state->common = UDFState_alloc(num_in_cols, arg_col_map, in_types, out_type);

    /* UDF directory must be in the PYTHONPATH */
    snprintf(buf, sizeof(buf), "import os\nimport sys\nsys.path.append(\"%s\")", UDFFile_dirname);
    PyRun_SimpleString(buf);

    pName = PyUnicode_DecodeFSDefault(UDFFile_basename);
    state->pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (state->pModule == NULL) {
        fprintf(stderr, "Fatal error: Cannot load Python module %s/%s.\n", UDFFile_dirname, UDFFile_basename);
        assert(0);
    }

    state->pFunc = PyObject_GetAttrString(state->pModule, UDF_MAIN_FUNCTION_NAME);
    /* pFunc is a new reference */

    if (!state->pFunc || !PyCallable_Check(state->pFunc)) {
        fprintf(stderr, "Fatal error: Provided function name %s is not callable.\n", fname);
        assert(0);
    }
    
#ifdef PROFILE
    profile_stop(op->profile_stats, BUILD);
#endif
    return op;
}