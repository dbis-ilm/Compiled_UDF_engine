#include "../src/engine.h"
#include "../test/paths.h"
int main() {
    init_paths();
    engine_start();
    
    Type schema[] = {STRING, STRING};
    Operator *query = scan_build(reviews_100, "|", 2, schema, exp_vectorsize);
    Type udf_in[] = {STRING};
    int arg_col_map[] = {1};
    query = pythonUDF_build(query, roberta, "roberta", 1, arg_col_map, udf_in, STRING);
    run_query(query, false);
    print_query_profile(query);
    query_close(query);

    engine_stop();
}

