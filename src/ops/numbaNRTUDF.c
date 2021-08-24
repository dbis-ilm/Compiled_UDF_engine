#include "numbaNRTUDF.h"
#include <dlfcn.h>
#include "../utils/UDFutils.h"
#include "../utils/fmultiplexer.h"

typedef PyMODINIT_FUNC (*InitPtr)(void);
typedef PyObject* (*FuncPtr)(PyObject*, PyObject*);

/** State of the Numba UDF execution */
struct NumbaNRTUDFState_t {
    /** Common UDF state properties */
    UDFState *common;
    /** Pointer to the Python module */
    PyObject *pModule;
    /** Pointer to the function pointer. Parameter are passed
     * as a PyTuple in the second argument.
     */
    FuncPtr thefunc;
};
typedef struct NumbaNRTUDFState_t NumbaNRTUDFState;

static void load_module(UDFInfo *info, NumbaNRTUDFState *state) {
    InitPtr Init;
    void *pMod;
    char *name = "UDF";
    PyObject* sys_modules;
    
    pMod = dlopen(info->libpath, RTLD_LAZY);
    if (!pMod) {
        printf("Error (%s) loading module\n", dlerror());
        assert(0);
        return;
    }

    state->thefunc = dlsym(pMod, info->function_symbol);
    if (!state->thefunc) {
        printf("Error (%s) loading function\n", dlerror());
        assert(0);
        dlclose(pMod);
        return;
    }
    Init = dlsym(pMod, info->init_func_symbol);
    if (!Init)
    {
        printf("Error (%s) loading function\n", dlerror());
        dlclose(pMod);
        assert(0);
        return;
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

static void numbaNRTUDF_close(Operator *op) {
    NumbaNRTUDFState *state = (NumbaNRTUDFState*) op->state;
    Py_DECREF(state->pModule);
    printf("%i out of %i UDF calls failed.\n", state->common->failed, state->common->processed);
    UDFState_free(state->common);
    free(state);
    operator_free(op);
}

static int numbaNRTUDF_next(Operator *op) {
    NumbaNRTUDFState *state = (NumbaNRTUDFState*) op->state;
    int i, k;
    int added_col_idx;
    PyObject *pArgs, *pValue;

    int n = op->child->next(op->child);
#ifdef PROFILE
    profile_start(op->profile_stats);
#endif
    op->data = op->child->data;

    op->data = reallocate_buffer(op->data, op->child->num_cols, op->num_cols, op->col_types, op->vectorsize);
    added_col_idx = op->num_cols - 1;

    // We can return only after reallocation, otherwise the buffer does not match op->num_cols
    if (n == 0) goto end;

    pArgs = PyTuple_New(state->common->num_in_cols);
    for (i = 0; i < n; i++) {
        /* Prepare the args */
        bool success = true;

        for (k = 0; k < state->common->num_in_cols; ++k) {
            int arg_col_idx = state->common->arg_col_map[k];
            char *elem = op->data[arg_col_idx][i];
            if (!elem) {
                success = false;
                break;
            }
            pValue = state->common->input_conversion[k](&elem);
            if (!pValue) {
                fprintf(stderr, "Cannot convert argument for row %i. Skipping UDF call\n", state->common->processed + i);
                state->common->failed++;
                success = false;
                break;
            }
            PyTuple_SetItem(pArgs, k, pValue);
        }

        /* Do not apply on Null values */
        if (!success) continue;

        /* Run */
        pValue = state->thefunc(NULL, pArgs);   

        /* Get returns */
        if (pValue != NULL) {
            state->common->output_conversion(&(op->data[added_col_idx][i]), pValue);
            Py_DECREF(pValue);
        }
        else {
            fprintf(stderr,"UDF Call failed for row %i\n", state->common->processed + i);
            PyErr_Print();
            state->common->failed++;
        } 
    }
    Py_DECREF(pArgs);
    state->common->processed += n;

end:
#ifdef PROFILE
    profile_stop(op->profile_stats, EXECUTE);
#endif
    return n;
}

Operator *numbaNRTUDF_build(Operator *child, char* UDFFile, char* fname, int num_in_cols, int *arg_col_map,
    Type *in_types, Type out_type) {
    Operator *op;
    NumbaNRTUDFState *state;
    UDFInfo *info;

    // TODO: Check UDFFile of type *.py
    op = operator_alloc(&numbaNRTUDF_next, &numbaNRTUDF_close, child, child->num_cols + 1, child->col_types, "NumbaUDF");

    /* Compile the UDF */
    info = alloc_UDFInfo();
    compile_numba_udf(UDFFile, fname, info, in_types, num_in_cols, out_type, true);
    assert(info->function_symbol);
    // TODO: Proper way out if compilation failed.

    /* UDF adds a column */
    
    op->col_types[op->num_cols - 1] = out_type;
    op->state = malloc(sizeof(NumbaNRTUDFState));
    state = (NumbaNRTUDFState*) op->state;
    state->common = UDFState_alloc(num_in_cols, arg_col_map, in_types, out_type);
    load_module(info, state);
    free_UDFInfo(info);

#ifdef PROFILE
    profile_stop(op->profile_stats, BUILD);
#endif
    return op;
}