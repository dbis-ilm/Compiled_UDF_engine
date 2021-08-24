#include "../src/engine.h"
#include "paths.h"
int main() {
    init_paths();
    engine_start();

    Type schema[] = {INT, INT, DOUBLE, STRING};
    Operator *query = scan_build(testdata, "|", 4, schema, test_vectorsize);
    Type udf_in[] = {STRING};
    int arg_col_map[] = {3};
    query = pythonUDF_build(query, concat_timestamp, "print_time", 1, arg_col_map, udf_in, STRING);
    run_query(query, true);
    print_query_profile(query);
    query_close(query);

    engine_stop();
}

