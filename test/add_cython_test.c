#include "../src/engine.h"
#include "paths.h"
int main() {
    init_paths();
    engine_start();

    Type schema[] = {INT, INT, DOUBLE, STRING};
    Operator *query = scan_build(testdata, "|", 4, schema, test_vectorsize);
    Type udf_in[] = {INT, INT};
    int arg_col_map[] = {0, 1};
    query = cythonUDF_build(query, add_pyx, "add", 2, arg_col_map, udf_in, INT);
    run_query(query, true);
    print_query_profile(query);
    query_close(query);

    engine_stop();
}

