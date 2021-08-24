#include "../src/engine.h"
#include "paths.h"
int main() {
    init_paths();
    engine_start();
    
    Type schema[] = {INT, INT, DOUBLE, STRING};
    Operator *query = scan_build(testdata, "|", 4, schema, test_vectorsize);
    Type udf_in[] = {INT};
    int arg_col_map[] = {0};
    query = numbaUDF_build(query, mod_numba, "mod", 1, arg_col_map, udf_in, INT);
    run_query(query, true);
    print_query_profile(query);
    query_close(query);

    engine_stop();
}

