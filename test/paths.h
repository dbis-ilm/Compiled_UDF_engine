#include <stdlib.h>
#include <string.h>

// VECTORSIZE
int test_vectorsize = 4;
int exp_vectorsize = 1024;

// DATA
char reviews[1024];
char reviews_10[1024];
char reviews_100[1024];
char testdata[1024];
char partsupp[1024];

// UDFs
char avg_word_len[1024];
char avg_word_len_numba[1024];
char avg_word_len_py[1024];
char avg_word_len_pyx[1024];
char vectorized_avg_word_len[1024];
char vectorized_avg_word_len_py[1024];
char vectorized_avg_word_len_pyx[1024];

char concat_timestamp[1024];
char concat_timestamp_py[1024];
char concat_timestamp_pyx[1024];
char vectorized_concat_timestamp[1024];
char vectorized_concat_timestamp_py[1024];
char vectorized_concat_timestamp_pyx[1024];

char concat_strings[1024];
char concat_strings_numba[1024];
char concat_strings_py[1024];
char concat_strings_pyx[1024];
char vectorized_concat_strings[1024];
char vectorized_concat_strings_py[1024];
char vectorized_concat_strings_pyx[1024];

char mod[1024];
char mod_numba[1024];
char mod_py[1024];
char mod_pyx[1024];
char vectorized_mod[1024];
char vectorized_mod_py[1024];
char vectorized_mod_pyx[1024];

char add[1024];
char add_numba[1024];
char add_py[1024];
char add_pyx[1024];
char vectorized_add[1024];
char vectorized_add_py[1024];
char vectorized_add_pyx[1024];

char roberta[1024];
char roberta_py[1024];
char roberta_pyx[1024];
char vectorized_roberta[1024];
char vectorized_roberta_py[1024];
char vectorized_roberta_pyx[1024];

void init_paths() {
    char *test_dir = getenv("UDF_TEST_DIR");
    char *exp_dir = getenv("UDF_EXP_DIR");

    assert(exp_dir);
    assert(test_dir);
    
    strcpy(reviews, test_dir);
    strcat(reviews, "/data/reviews.csv");

    strcpy(reviews_10, test_dir);
    strcat(reviews_10, "/data/reviews_trunc.csv");

    strcpy(reviews_100, test_dir);
    strcat(reviews_100, "/data/reviews_100.csv");

    strcpy(testdata, test_dir);
    strcat(testdata, "/data/testdata.csv");

    strcpy(partsupp, exp_dir);
    strcat(partsupp, "/tpch-dbgen/partsupp.tbl");

    strcpy(avg_word_len, test_dir);
    strcat(avg_word_len, "/exampleUDFs/avg_word_len");

    strcpy(avg_word_len_numba, test_dir);
    strcat(avg_word_len_numba, "/exampleUDFs/avg_word_len_numba.py");

    strcpy(vectorized_avg_word_len, test_dir);
    strcat(vectorized_avg_word_len, "/exampleUDFs/vectorized_avg_word_len");

    strcpy(vectorized_avg_word_len_pyx, test_dir);
    strcat(vectorized_avg_word_len_pyx, "/exampleUDFs/vectorized_avg_word_len.pyx");

    strcpy(vectorized_avg_word_len_py, test_dir);
    strcat(vectorized_avg_word_len_py, "/exampleUDFs/vectorized_avg_word_len.py");

    strcpy(avg_word_len_py, test_dir);
    strcat(avg_word_len_py, "/exampleUDFs/avg_word_len.py");

    strcpy(avg_word_len_pyx, test_dir);
    strcat(avg_word_len_pyx, "/exampleUDFs/avg_word_len.pyx");

    strcpy(concat_timestamp, test_dir);
    strcat(concat_timestamp, "/exampleUDFs/concat_timestamp");

    strcpy(vectorized_concat_timestamp, test_dir);
    strcat(vectorized_concat_timestamp, "/exampleUDFs/vectorized_concat_timestamp");

    strcpy(vectorized_concat_timestamp_pyx, test_dir);
    strcat(vectorized_concat_timestamp_pyx, "/exampleUDFs/vectorized_concat_timestamp.pyx");

    strcpy(vectorized_concat_timestamp_py, test_dir);
    strcat(vectorized_concat_timestamp_py, "/exampleUDFs/vectorized_concat_timestamp.py");

    strcpy(concat_timestamp_py, test_dir);
    strcat(concat_timestamp_py, "/exampleUDFs/concat_timestamp.py");

    strcpy(concat_timestamp_pyx, test_dir);
    strcat(concat_timestamp_pyx, "/exampleUDFs/concat_timestamp.pyx");

    strcpy(concat_strings, test_dir);
    strcat(concat_strings, "/exampleUDFs/concat_strings");

    strcpy(concat_strings_numba, test_dir);
    strcat(concat_strings_numba, "/exampleUDFs/concat_strings_numba.py");

    strcpy(vectorized_concat_strings, test_dir);
    strcat(vectorized_concat_strings, "/exampleUDFs/vectorized_concat_strings");

    strcpy(vectorized_concat_strings_pyx, test_dir);
    strcat(vectorized_concat_strings_pyx, "/exampleUDFs/vectorized_concat_strings.pyx");

    strcpy(vectorized_concat_strings_py, test_dir);
    strcat(vectorized_concat_strings_py, "/exampleUDFs/vectorized_concat_strings.py");

    strcpy(concat_strings_py, test_dir);
    strcat(concat_strings_py, "/exampleUDFs/concat_strings.py");

    strcpy(concat_strings_pyx, test_dir);
    strcat(concat_strings_pyx, "/exampleUDFs/concat_strings.pyx");

    strcpy(mod, test_dir);
    strcat(mod, "/exampleUDFs/mod");

    strcpy(mod_numba, test_dir);
    strcat(mod_numba, "/exampleUDFs/mod_numba.py");

    strcpy(vectorized_mod, test_dir);
    strcat(vectorized_mod, "/exampleUDFs/vectorized_mod");

    strcpy(vectorized_mod_pyx, test_dir);
    strcat(vectorized_mod_pyx, "/exampleUDFs/vectorized_mod.pyx");

    strcpy(vectorized_mod_py, test_dir);
    strcat(vectorized_mod_py, "/exampleUDFs/vectorized_mod.py");

    strcpy(mod_py, test_dir);
    strcat(mod_py, "/exampleUDFs/mod.py");

    strcpy(mod_pyx, test_dir);
    strcat(mod_pyx, "/exampleUDFs/mod.pyx");

    strcpy(add, test_dir);
    strcat(add, "/exampleUDFs/add");

    strcpy(add_numba, test_dir);
    strcat(add_numba, "/exampleUDFs/add_numba.py");

    strcpy(vectorized_add, test_dir);
    strcat(vectorized_add, "/exampleUDFs/vectorized_add");

    strcpy(vectorized_add_pyx, test_dir);
    strcat(vectorized_add_pyx, "/exampleUDFs/vectorized_add.pyx");

    strcpy(vectorized_add_py, test_dir);
    strcat(vectorized_add_py, "/exampleUDFs/vectorized_add.py");

    strcpy(add_py, test_dir);
    strcat(add_py, "/exampleUDFs/add.py");

    strcpy(add_pyx, test_dir);
    strcat(add_pyx, "/exampleUDFs/add.pyx");

    strcpy(roberta, test_dir);
    strcat(roberta, "/exampleUDFs/roberta");
    
    strcpy(roberta_py, test_dir);
    strcat(roberta_py, "/exampleUDFs/roberta.py");

    strcpy(roberta_pyx, test_dir);
    strcat(roberta_pyx, "/exampleUDFs/roberta.pyx");

    strcpy(vectorized_roberta, test_dir);
    strcat(vectorized_roberta, "/exampleUDFs/vectorized_roberta");
    
    strcpy(vectorized_roberta_py, test_dir);
    strcat(vectorized_roberta_py, "/exampleUDFs/vectorized_roberta.py");

    strcpy(vectorized_roberta_pyx, test_dir);
    strcat(vectorized_roberta_pyx, "/exampleUDFs/vectorized_roberta.pyx");
}
