#include "src/engine.h"
#include "test/paths.h"
int main() {
    init_paths();
    engine_start();

    Type schema[] = {INT, INT, DOUBLE, STRING};
    Operator *query = scan_build(testdata, "|", 4, schema, test_vectorsize);
    Type udf_in[] = {INT, INT};
    int arg_col_map[] = {0, 1};
    query = nuitkaUDF_build(query, add_py, "add", 2, arg_col_map, udf_in, INT);
    Type udf2_in[] = {INT};
    int arg_col_map2[]= {4};
    query = nuitkaUDF_build(query, mod_py, "square", 1, arg_col_map2, udf2_in, INT);
    run_query(query, true);
    print_query_profile(query);
    query_close(query);


    query = scan_build(testdata, "|", 4, schema, test_vectorsize);
    Type udf_in3[] = {STRING};
    int arg_col_map3[] = {3};
    query = nuitkaUDF_build(query, concat_timestamp_py, "print_time", 1, arg_col_map3, udf_in3, STRING);
    run_query(query, true);
    print_query_profile(query);
    query_close(query);

    engine_stop();
}
