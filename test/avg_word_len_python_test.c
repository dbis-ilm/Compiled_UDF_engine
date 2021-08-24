#include "../src/engine.h"
#include "paths.h"
int main() {
    init_paths();
    engine_start();

    Type schema[] = {STRING, STRING};
    Operator *query = scan_build(reviews_10, "|", 2, schema, test_vectorsize);

    Type udf_in[] = {STRING};
    int arg_col_map[] = {1};
    query = pythonUDF_build(query, avg_word_len, "avg_word_len", 1, arg_col_map, udf_in, DOUBLE);
    run_query(query, true);
    print_query_profile(query);
    query_close(query);

    engine_stop();
}

