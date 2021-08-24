#include "cythonUDF.h"
#include <dlfcn.h>
#include "../utils/UDFutils.h"

typedef PyMODINIT_FUNC (*InitPtr)(void);

// TODO: Generate
typedef PyObject* (*oo)(PyObject*);
typedef PyObject* (*ooo)(PyObject*, PyObject*);
typedef void (*fplaceholder)(void);
typedef PyObject* (*WrapperFunc)(fplaceholder, PyObject**);

PyObject* func_oo(fplaceholder f, PyObject **params) {
    PyObject *p1 = params[0];
    return ((oo)f)(p1);
}

PyObject* func_ooo(fplaceholder f, PyObject **params) {
    PyObject *p1 = params[0];
    PyObject *p2 = params[1];
    return ((ooo)f)(p1, p2);
}

/** State of the Cython UDF execution */
struct CythonUDFState_t {
    /** Common UDF state properties */
    UDFState *common;
    /** Pointer to the UDF module */
    PyObject *pModule;
    /** Function pointer of generic type. Needs to be casted correctly before execution */
    fplaceholder thefunc;
    /** Pointer to the wrapper function that handles inputs, casts the actual function pointer and calls the function */
    WrapperFunc wrapper;
};

typedef struct CythonUDFState_t CythonUDFState;

static void load_module(UDFInfo *info, CythonUDFState *state) {
    void *pMod = dlopen(info->libpath, RTLD_LAZY);
    InitPtr Init;
    PyObject* sys_modules;
    char *name = "UDF";
    if (!pMod) {
        printf("Error (%s) loading module\n", dlerror());
        assert(0);
    }
    Init = dlsym(pMod, info->init_func_symbol);
    if (!Init)
    {
        printf("Error (%s) loading function\n", dlerror());
        dlclose(pMod);
        assert(0);
    }
    state->thefunc = (fplaceholder)dlsym(pMod, info->function_symbol);
    if (!state->thefunc) {
        printf("Error (%s) loading function\n", dlerror());
        dlclose(pMod);
        assert(0);
    }

    PyImport_AddModule(name);
    state->pModule = Init();
    sys_modules = PyImport_GetModuleDict();
    PyDict_SetItemString(sys_modules, name, state->pModule);

    // PEP489_MULTI_PHASE_INIT
    if (!PyModule_Check(state->pModule)) {
        PyModuleDef *mdef = (PyModuleDef *) state->pModule;
        PyObject *modname = PyUnicode_FromString("__main__");
        state->pModule = NULL;
        if (modname) {
            state->pModule = PyModule_NewObject(modname);
            Py_DECREF(modname);
            if (state->pModule) PyModule_ExecDef(state->pModule, mdef);
        }
    }
    if (PyErr_Occurred()) {
        PyErr_Print();
        assert(0);
    }
}

static WrapperFunc get_wrapper(int num_params) {
    switch (num_params)
    {
    case 1:
        return func_oo;
    case 2:
        return func_ooo;
    
    default:
        assert(0);
    }
}

static void cythonUDF_close(Operator *op) {
    CythonUDFState *state = (CythonUDFState*) op->state;
    Py_DECREF(state->pModule);
    printf("%i out of %i UDF calls failed.\n", state->common->failed, state->common->processed);
    UDFState_free(state->common);
    free(state);
    operator_free(op);
}


static int cythonUDF_next(Operator *op) {
    CythonUDFState *state = (CythonUDFState*) op->state;
    int i, k;
    int added_col_idx;
    PyObject *pValue, *pRet = NULL;
    PyObject **pArgs = NULL;

    int n = op->child->next(op->child);
#ifdef PROFILE
    profile_start(op->profile_stats);
#endif
    op->data = op->child->data;
    
    op->data = reallocate_buffer(op->data, op->child->num_cols, op->num_cols, op->col_types, op->vectorsize);
    added_col_idx = op->num_cols - 1;

    // We can return only after reallocation, otherwise the buffer does not match op->num_cols
    if (n == 0) goto end;

    /* Prepare the args by iterating over a vector of each column*/    
    pArgs = malloc(state->common->num_in_cols * sizeof(PyObject*));
    for (k = 0; k < state->common->num_in_cols; ++k) {
        int arg_col_idx = state->common->arg_col_map[k];
        pArgs[k] = PyList_New(n);
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
            PyList_SetItem(pArgs[k], i, pValue);
        }
    }

    /* Run */
    pRet = state->wrapper(state->thefunc, pArgs);   

    if (pRet != NULL) {
        for (i = 0; i < n; i++) {
            pValue = PyList_GetItem(pRet, i);
            if (pValue != Py_None) {
                state->common->output_conversion(&(op->data[added_col_idx][i]), pValue);
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
    if (pArgs) {
        for (k = 0; k < state->common->num_in_cols; ++k) {
            Py_XDECREF(pArgs[k]);
        }
        free(pArgs);
    }
    Py_XDECREF(pRet);
#ifdef PROFILE
    profile_stop(op->profile_stats, EXECUTE);
#endif
    return n;
}

Operator *cythonUDF_build(Operator *child, char* UDFFile, char* fname, int num_in_cols, int *arg_col_map,
    Type *in_types, Type out_type) {
    Operator *op;
    CythonUDFState *state;
    UDFInfo *info;
    bool is_vectorized = UDF_is_vectorized(fname);
    nextfunc next = cythonUDF_next;
    char *opname = is_vectorized ? "CythonUDF(vec)" : "CythonUDF";
    // TODO: Check UDFFile of type *.pyx

    op = operator_alloc(next, &cythonUDF_close, child, child->num_cols + 1, child->col_types, opname);

    /* Compile the UDF */
    info = alloc_UDFInfo();
    compile_cython_udf(UDFFile, fname, info);
    assert(info->function_symbol);
    // TODO: Proper way out if compilation failed.
    
    /* UDF adds a column */
    op->col_types[op->num_cols - 1] = out_type;
    op->state = calloc(1, sizeof(CythonUDFState));
    state = ((CythonUDFState*)(op->state));
    state->common = UDFState_alloc(num_in_cols, arg_col_map, in_types, out_type);

    load_module(info, state);

    state->wrapper = get_wrapper(num_in_cols);

    free_UDFInfo(info);
   
#ifdef PROFILE
    profile_stop(op->profile_stats, BUILD);
#endif
    return op;
}