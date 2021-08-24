#include "engine.h"

#ifdef PROFILE
#include "profile.h"
#endif

static void print_value_of_type(char** value, Type t) {
    switch (t)
    {
    case INT:
        printf("%li", *(long*)value);
        break;

    case DOUBLE:
        printf("%f", *(double*)value);
        break;

    case STRING:
        printf("%s", *value);
        break; 
    default:
        assert(0);
        break;
    }
}

void run_query(Operator *root, bool print) {
    int n = root->next(root);
    int i, k;
    long num_results = n;
    while (n > 0) {
        if (print) {
            for (i = 0; i < n; i++) {
                for (k = 0; k < root->num_cols; k++) {
                    print_value_of_type(&(root->data[k][i]), root->col_types[k]);
                    printf(" | ");
                }
                printf("\n");
            }
        }
        
        free_buffer(root->data, root->num_cols, root->col_types, n);
        n = root->next(root);
        num_results += n;
    }
    free_buffer(root->data, root->num_cols, root->col_types, n); // Free the last empty buffer
    printf("(%li results)\n", num_results);
}

void print_query_profile(Operator *root) {
#ifdef PROFILE
    Operator *op = root;
    ProfileStats *total = profile_alloc();
    assert(root);

    printf("\nQuery Profile (Vectorsize: %i) \n", root->vectorsize);
    printf("************* \n");

    while (op) {
        profile_print(op->profile_stats, op->name, total);
        op = op->child;
    }
    printf("------------- \n");
    profile_print(total, "TOTAL", NULL);
    profile_free(total);
#else
    printf("No profile information collected\n");
#endif
}

void query_close(Operator *root) {
    Operator *op = root;
    while (op) {
        Operator *child = op->child;
        op->close(op);
        op = child;
    }
}

void engine_start() {
    Py_Initialize();
}

void engine_stop() {
    /* Make finalizing with threads possible. 
    https://stackoverflow.com/questions/27844676/assertionerror-3-x-only-when-calling-py-finalize-with-threads
    */
    Py_DECREF(PyImport_ImportModule("threading"));
    if (Py_FinalizeEx() < 0) {
        fprintf(stderr, "Fatal error: Py_FinalizeEx failed.\n");
        assert(0);
    }
}