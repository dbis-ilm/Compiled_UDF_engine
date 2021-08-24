#include "../src/engine.h"
#include "../test/paths.h"
int main() {
    init_paths();
    engine_start();
    
    Type schema[] = {INT, INT, INT, DOUBLE, STRING};
    Operator *query = scan_build(partsupp, "|", 5, schema, exp_vectorsize);

    Type udf_in[] = {STRING};
    int arg_col_map[] = {4};
    query = nuitkaUDF_build(query, avg_word_len_py, "avg_word_len", 1, arg_col_map, udf_in, DOUBLE);
    run_query(query, false);
    print_query_profile(query);
    query_close(query);

    engine_stop();
}

