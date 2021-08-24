#include "../src/engine.h"
#include "paths.h"
int main() {
    init_paths();
    engine_start();

    Type schema[] = {STRING, STRING};
    Operator *query = scan_build(reviews_10, "|", 2, schema, test_vectorsize);
    Type udf_in[] = {STRING};
    int arg_col_map[] = {1};
    query = nuitkaUDF_build(query, vectorized_roberta_py, "vectorized_roberta", 1, arg_col_map, udf_in, STRING);
    run_query(query, true);
    print_query_profile(query);
    query_close(query);

    engine_stop();
}

