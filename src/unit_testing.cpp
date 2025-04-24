#include "unit_testing.h"

const Test_Hashtable_Funcs_Set Hashtable_Funcs_Set[] = {
    {&insert, &search},
    {&insert_AVX, &search_AVX},
    {&insert_preload, &search_preload},
};

const size_t NUM_OF_SETS_FOR_TEST = sizeof(Hashtable_Funcs_Set) / sizeof(Hashtable_Funcs_Set[0]);

const size_t benchmark_index = 0;
const char dump_file_name[] = "run_results.txt";

void pin_to_core(int core_id) {
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(core_id, &set);
    sched_setaffinity(0, sizeof(set), &set);
}

void run_tests(size_t NUM_OF_TESTS) {

    FILE* fp_for_insert = fopen(processed_text, "r");
    if (fp_for_insert == NULL) { perror("Stream for insert test not opened"); }
    FILE* fp_for_search = fopen(test_for_search, "r");
    if (fp_for_search == NULL) { perror("Stream for search test not opened"); }

    printf("Number of functions to test: %zu\n", NUM_OF_SETS_FOR_TEST);

    uint64_t insert_test_results[NUM_OF_SETS_FOR_TEST] = {};
    uint64_t search_test_results[NUM_OF_SETS_FOR_TEST] = {};

    uint64_t normilized_test_results[NUM_OF_SETS_FOR_TEST] = {};
    uint64_t temp_res = 0;

    uint64_t start = 0;
    uint64_t end   = 0;

    size_t insert_ticks_counter = 0;
    size_t search_ticks_counter = 0;

    hashtable_t htbl = {};
    char buffer[STRING_SIZE] = {};

    for (size_t func_num = 0; func_num < NUM_OF_SETS_FOR_TEST; func_num++) {

        htbl = init();
        insert_ticks_counter = 0;
        search_ticks_counter = 0;

        htbl.insert = Hashtable_Funcs_Set[func_num].insert;
        htbl.search = Hashtable_Funcs_Set[func_num].search;

        for (size_t test_num = 0; test_num < NUM_OF_TESTS; test_num++) {
            while (fgets(buffer, STRING_SIZE, fp_for_insert)) {
                _mm_lfence();
                start = rdtsc();
                htbl.insert(&htbl, buffer);
                _mm_lfence();
                end = rdtsc();
                insert_ticks_counter += (end - start);
            }

            while (fgets(buffer, STRING_SIZE, fp_for_search)) {
                _mm_lfence();
                start = rdtsc();
                htbl.search(&htbl, buffer);
                _mm_lfence();
                end = rdtsc();
                search_ticks_counter += (end - start);
            }

            fseek(fp_for_insert, 0, SEEK_SET);
            fseek(fp_for_search, 0, SEEK_SET);
        }

        printf( "func number:  %ld\n"
                "insert ticks: %ld\n"
                "search ticks: %ld\n\n",
                func_num, insert_ticks_counter, search_ticks_counter);

        insert_test_results[func_num] = insert_ticks_counter;
        search_test_results[func_num] = search_ticks_counter;

        destroy_hashtable(&htbl);
    }

    double insert_benchmark_elem = static_cast<double>(insert_test_results[benchmark_index]);
    double search_benchmark_elem = static_cast<double>(search_test_results[benchmark_index]);

    printf("-----Testing results-----\n");
    printf("Number of processor cycles per function:\n");
    printf("            |             Insert         |            Search          |\n");
    printf("Func Number | ticks          | benckmark | ticks          | benckmark |\n");

    for (size_t i = 0; i < NUM_OF_SETS_FOR_TEST; i++) {
        printf("%11zu | %14lu | %9lf | %14lu | %9lf |\n",
                i, insert_test_results[i], search_test_results[i], insert_benchmark_elem / insert_test_results[i], search_benchmark_elem / search_test_results[i]);
    }

    fclose(fp_for_insert);
    fclose(fp_for_search);

    //file_dump(test_results);
}

bool check_word (hashtable_t* htbl, const char* word) {
    assert(htbl);
    assert(word);

    FILE* fp = fopen(processed_text, "r");
    if (fp == NULL) { perror("Stream for insert test not opened"); }

    char temp_buf[STRING_SIZE] = {};

    uint64_t text_counter = 0;
    while (fgets(temp_buf, STRING_SIZE, fp)) {
        if (strncmp(temp_buf, word, STRING_SIZE) == 0) {
            text_counter++;
        }
    }

    fclose(fp);

    strncpy(temp_buf,  word, STRING_SIZE);

    hashtable_elem_t* search_res = search(htbl, temp_buf);
    if (search_res) {

        printf( "\"%s\" count:\n"
                "text:      %ld\n"
                "hashtable: %ld\n",
                word, text_counter, search_res -> counter);
    }
    else {
        fprintf(stderr, "Could not find \".....\" in hash table\n");
        printf( "\"%s\" count:"
                "text:      %ld\n", text_counter);

        return false;
    }

    return text_counter == (search_res -> counter);
}
