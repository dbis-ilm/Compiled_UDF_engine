#include "UDFutils.h"

UDFInfo *alloc_UDFInfo() {
    UDFInfo *ret = malloc(sizeof(UDFInfo));
    ret->buffsize = 1000;
    ret->libpath = malloc(ret->buffsize * sizeof(char));
    ret->function_symbol = malloc(ret->buffsize * sizeof(char));
    ret->init_func_symbol = NULL;
    return ret;
}

void free_UDFInfo(UDFInfo *info) {
    free(info->libpath);
    free(info->function_symbol);
    if (info->init_func_symbol) free(info->init_func_symbol);
    free(info);
}

UDFState *UDFState_alloc(int num_in_cols, int *arg_col_map, Type *in_types, Type out_type) {
    UDFState *state = malloc(sizeof(UDFState));
    state->num_in_cols = num_in_cols;
    state->arg_col_map = malloc(num_in_cols * sizeof(int));
    memcpy(state->arg_col_map, arg_col_map, num_in_cols * sizeof(int));
    state->in_types = in_types;
    state->out_type = out_type;
    state->processed = 0;
    state->failed = 0;
    state->input_conversion = get_input_conversions(in_types, num_in_cols);
    state->output_conversion = get_output_conversion(out_type);
    return state;
}

void UDFState_free(UDFState *state) {
    free(state->arg_col_map);
    free(state);
}

static PyObject *long_to_pylong(char** value) {
    long v = *((long*)value);
    return PyLong_FromLong(v);
}

static PyObject *double_to_pyfloat(char** value) {
    double v = *((double*)value);
    return PyFloat_FromDouble(v);
}

static PyObject *string_to_pyunicode(char** value) {
    return PyUnicode_FromString(*value);
}

static void long_from_pylong(char **buffer, PyObject *value) {
    long val = PyLong_AsLong(value);
    *((long*)buffer) = val;
}

static void double_from_pyfloat(char **buffer, PyObject *value) {
    double val = PyFloat_AsDouble(value);
    *((double*)buffer) = val;
}

static void string_from_pyunicode(char **buffer, PyObject *value) {
    Py_ssize_t ret_size;
    char *val = PyUnicode_AsUTF8AndSize(value, &ret_size);
    *buffer = calloc(ret_size, sizeof(char));
    strncpy(*buffer, val, ret_size);
}

Convert_to_PyObject *get_input_conversions(Type *col_types, int num_cols) {
    Convert_to_PyObject *ret = calloc(num_cols, sizeof(Convert_to_PyObject));
    int i;

    for (i = 0; i < num_cols; i++) {
        switch (col_types[i]) {
            case INT:
                ret[i] = &long_to_pylong;
                break;

            case DOUBLE:
                ret[i] = &double_to_pyfloat;
                break;

            case STRING:
                ret[i] = &string_to_pyunicode;
                break;
            
            default:
                assert(0);
                break;
        }
    }
    return ret;
}

Convert_from_PyObject_and_fill get_output_conversion(Type ret_type) {
    switch (ret_type) {
        case INT:
            return &long_from_pylong;

        case DOUBLE:
            return &double_from_pyfloat;

        case STRING:
            return &string_from_pyunicode;
        
        default:
            assert(0);
            break;
    }
}

bool UDF_is_vectorized(char *fname){
    char *prefix = "vectorized";
    return strncmp(prefix, fname, strlen(prefix)) == 0;
}

static const char *type_to_numba_type(Type t) {
    switch (t)
    {
    case INT:
        return "i4";
    case DOUBLE:
        return "f8";
    case STRING:
        return "unicode_type";

    default:
        fprintf(stderr, "Unknown type");
        assert(0);
    }
}

static void format_numba_signature(char **signature, int buffersize,
    Type *in_types, int num_in_types, Type out_type) {
    int i;
    char *sig = *signature;
    const char *out = type_to_numba_type(out_type);
    const char *in = type_to_numba_type(in_types[0]);
    snprintf(sig, buffersize, "\"%s(%s", out, in);

    for (i = 1; i < num_in_types; i++) {
        in = type_to_numba_type(in_types[i]);
        snprintf(&sig[strlen(sig)], buffersize - strlen(sig), ", %s", in);
    }
    snprintf(&sig[strlen(sig)], buffersize - strlen(sig), ")\"");
}

void compile_numba_udf(char *UDFFile, char *fname, UDFInfo *info,
    Type *in_types, int num_in_cols, Type out_type, bool NRT_necessary) {
    int buffsize = 1000;
    char* cmd = malloc(buffsize * sizeof(char));
    char *log_file = "/tmp/numba.log";
    const char* utils_path = getenv("UDFUTILS");
    char *numba_signature = malloc(buffsize * sizeof(char));
    FILE *fp;
    int initsymbol_len;
    errno = 0;

    if (!utils_path) {
        fprintf(stderr, "UDFUTILS environment variable not set\n");
        free(numba_signature);
        free(cmd);
        return;
    }

    if( access( UDFFile, F_OK ) != 0 ) {
        fprintf(stderr, "UDFFile does not exist\n");
        free(numba_signature);
        free(cmd);
        assert(0);
    }

    format_numba_signature(&numba_signature, buffsize, in_types, num_in_cols, out_type);

    snprintf(info->libpath, info->buffsize, "/tmp/%s.so", fname);
    snprintf(cmd, buffsize, "%s/compile_numba.py "\
        "%s %s %s %s %d 2>%s", utils_path, UDFFile, fname, info->libpath,\
        numba_signature, NRT_necessary, log_file);
    /* Open the command for reading. */
    fp = popen(cmd, "r");
    free(numba_signature);
    free(cmd);

    if (fp != NULL) {
        /* Compile script has single line output with function symbol */
        char *sym = fgets(info->function_symbol, info->buffsize, fp);
        assert(sym);
        /* close */
        pclose(fp);
    } else {
        int compile_return = WEXITSTATUS(pclose(fp));

        switch (compile_return)
        {
        case -1:
            printf("System call failed. Errno is %i\n", errno);
            return;
        case 0:
            printf("Numba compilation succeeded\n");
            break;
        case 1:
            fprintf(stderr, "Wrong compilation script arguments\n");
            return;
        case 2:
            fprintf(stderr, "Numba compilation failed\n");
            return;
        case 3:
            fprintf(stderr, "Changing function visibility failed\n");
            return;
        case 4:
            fprintf(stderr, "Compile failed\n");
            return;
        default:
            fprintf(stderr, "Unknown error code\n");
            return;
        }
    }

    /* get the init_func_symbol */
    initsymbol_len = strlen(fname) + 8;
    info->init_func_symbol = calloc(initsymbol_len, sizeof(char));
    snprintf(info->init_func_symbol, initsymbol_len, "PyInit_%s", fname);
}

void compile_nuitka_udf(char *UDFFile, char *fname, UDFInfo *info) {
    int buffsize = 1000;
    char* cmd = malloc(buffsize * sizeof(char));
    char *log_file = "/tmp/nuitka.log";
    const char* utils_path = getenv("UDFUTILS");
    FILE *fp;
    char *UDFFile_copy;
    char *module_name;
    int initsymbol_len;
    errno = 0;

    if (!utils_path) {
        fprintf(stderr, "UDFUTILS environment variable not set\n");
        free(cmd);
        return;
    }

    if( access( UDFFile, F_OK ) != 0 ) {
        fprintf(stderr, "UDFFile does not exist\n");
        free(cmd);
        assert(0);
    }

    snprintf(info->libpath, info->buffsize, "/tmp/%s.so", fname);
    snprintf(cmd, buffsize, "%s/compile_nuitka.py "\
        "%s %s %s 2>%s", utils_path, UDFFile, UDF_MAIN_FUNCTION_NAME, info->libpath, log_file);

    /* Open the command for reading. */
    fp = popen(cmd, "r");
    free(cmd);

    if (fp != NULL) {
        /* Compile script has single line output with function symbol */
        char *sym = fgets(info->function_symbol, info->buffsize, fp);
        assert(sym);
        /* close */
        pclose(fp);
    } else {
        int compile_return = WEXITSTATUS(pclose(fp));

        switch (compile_return)
        {
        case -1:
            printf("System call failed. Errno is %i\n", errno);
            return;
        case 0:
            printf("Numba compilation succeeded\n");
            break;
        case 1:
            fprintf(stderr, "Wrong compilation script arguments\n");
            return;
        case 2:
            fprintf(stderr, "Numba compilation failed\n");
            return;
        case 3:
            fprintf(stderr, "Changing function visibility failed\n");
            return;
        case 4:
            fprintf(stderr, "Compile failed\n");
            return;
        default:
            fprintf(stderr, "Unknown error code\n");
            return;
        }
    }

    /* get the init_func_symbol */
    UDFFile_copy = calloc(strlen(UDFFile), sizeof(char));
    strncpy(UDFFile_copy, basename(UDFFile), strlen(UDFFile));

    module_name = strtok(UDFFile_copy, ".");
    initsymbol_len = strlen(module_name) + 8;
    info->init_func_symbol = calloc(initsymbol_len, sizeof(char));
    snprintf(info->init_func_symbol, initsymbol_len, "PyInit_%s", module_name);
    free(UDFFile_copy);
}

void compile_cython_udf(char* UDFFile, char* fname, UDFInfo *info) {
    int buffsize = 1000;
    char* cmd = malloc(buffsize * sizeof(char));
    char *log_file = "/tmp/cython.log";
    const char* utils_path = getenv("UDFUTILS");
    FILE *fp;
    char *UDFFile_copy;
    char *module_name;
    int initsymbol_len;
    errno = 0;

    if (!utils_path) {
        fprintf(stderr, "UDFUTILS environment variable not set\n");
        free(cmd);
        return;
    }

    if( access( UDFFile, F_OK ) != 0 ) {
        fprintf(stderr, "UDFFile does not exist\n");
        free(cmd);
        assert(0);
    }

    snprintf(info->libpath, info->buffsize, "/tmp/%s.so", fname);
    snprintf(cmd, buffsize, "%s/compile_cython.py "\
        "%s %s %s 2>%s", utils_path, UDFFile, UDF_MAIN_FUNCTION_NAME, info->libpath, log_file);

    /* Open the command for reading. */
    fp = popen(cmd, "r");
    free(cmd);
    if (fp != NULL) {
        /* Compile script has single line output with function symbol */
        char *sym = fgets(info->function_symbol, info->buffsize, fp);
        assert(sym);
        /* close */
        pclose(fp);
    } else {
        int compile_return = WEXITSTATUS(pclose(fp));

        switch (compile_return)
        {
        case -1:
            printf("System call failed. Errno is %i\n", errno);
            return;
        case 0:
            printf("Cython compilation succeeded\n");
            break;
        case 1:
            fprintf(stderr, "Wrong compilation script arguments\n");
            return;
        case 2:
            fprintf(stderr, "Cythonization failed\n");
            return;
        case 3:
            fprintf(stderr, "Getting the python flags failed\n");
            return;
        case 4:
            fprintf(stderr, "Compiling the library failed\n");
            return;
        case 5:
            fprintf(stderr, "Linking the library failed\n");
            return;
        default:
            fprintf(stderr, "Unknown error code\n");
            return;
        }
    }

    /* get the init_func_symbol */
    UDFFile_copy = calloc(strlen(UDFFile), sizeof(char));
    strncpy(UDFFile_copy, basename(UDFFile), strlen(UDFFile));

    module_name = strtok(UDFFile_copy, ".");
    initsymbol_len = strlen(module_name) + 8;
    info->init_func_symbol = calloc(initsymbol_len, sizeof(char));
    snprintf(info->init_func_symbol, initsymbol_len, "PyInit_%s", module_name);
    free(UDFFile_copy);
}
