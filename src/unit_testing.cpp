#include "unit_testing.h"

const Test_Hashtable_Funcs_Set Hashtable_Funcs_Set[] = {
    {&insert, &search, &list_search,            &crc32},
    {&insert, &search, &list_search_asm_opt,    &crc32},
    {&insert, &search, &list_search_asm,        &crc32},
    {&insert, &search, &list_search_asm,        &crc32_32},
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

// increases counter by ticks of operation execution:
#define RDTSC_MEASURE(operation, counter)                   \
    _mm_lfence();                                           \
    start = rdtsc();                                        \
    operation;                                              \
    _mm_lfence();                                           \
    end = rdtsc();                                          \
    counter += (end - start)                                // needs ';' after using define

void run_tests(size_t num_of_tests) {

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
    alignas(32) char buffer[STRING_SIZE] = {};


    htbl = init();
    while (fgets(buffer, STRING_SIZE, fp_for_insert)) {
        htbl.insert(&htbl, buffer);
    }

    for (size_t func_num = 0; func_num < NUM_OF_SETS_FOR_TEST; func_num++) {

        insert_ticks_counter = 0;
        search_ticks_counter = 0;

        //htbl.insert      = Hashtable_Funcs_Set[func_num].insert;
        htbl.search      = Hashtable_Funcs_Set[func_num].search;
        htbl.list_search = Hashtable_Funcs_Set[func_num].list_search;
        htbl.hash_func   = Hashtable_Funcs_Set[func_num].hash_func;


        //fseek(fp_for_insert, 0, SEEK_SET);

        for (size_t test_num = 0; test_num < num_of_tests; test_num++) {
            while (fgets(buffer, STRING_SIZE, fp_for_search)) {
                RDTSC_MEASURE(htbl.search(&htbl, buffer), search_ticks_counter);
            }
            fseek(fp_for_search, 0, SEEK_SET);
        }

        printf( "func number:  %ld\n"
                "search ticks: %ld\n\n",
                func_num, search_ticks_counter);

        //insert_test_results[func_num] = insert_ticks_counter;
        search_test_results[func_num] = search_ticks_counter;

        //destroy_hashtable(&htbl);
    }

    //double insert_benchmark_elem = static_cast<double>(insert_test_results[benchmark_index]);
    double search_benchmark_elem = static_cast<double>(search_test_results[benchmark_index]);

    printf("-----Testing results-----\n");
    printf("Number of processor cycles per function:\n");
    printf("            |            Search          |\n");
    printf("Func Number | ticks          | benckmark |\n");

    for (size_t i = 0; i < NUM_OF_SETS_FOR_TEST; i++) {
        printf("%11zu | %14lu | %9lf |\n",
                i,
                search_test_results[i],
                search_benchmark_elem / search_test_results[i]);
    }

    if (fclose(fp_for_insert) != 0) { perror("Could not close file for insert test"); }
    if (fclose(fp_for_search) != 0) { perror("Could not close file for search test"); }

    //file_dump(test_results);
}

#undef RDTSC_MEASURE

bool check_word (hashtable_t* htbl, const char* word, FILE* html_stream) {
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

        char buffer[STRING_SIZE] = {};
        strncpy(buffer, word, STRING_SIZE);

        size_t word_len = strlen(buffer);
        memset(buffer + word_len, 0, STRING_SIZE - word_len);

        size_t hash = crc32_32(buffer);
        size_t bucket_index = hash % BUCKETS_NUM;

        list_dump(&(htbl -> buckets[bucket_index].data), html_stream);

        printf( "\"%s\" count:\n"
                "text:      %ld\n"
                "hashtable: %ld\n",
                word, text_counter, search_res -> counter);
    }
    else {
        fprintf(stderr, "Could not find \".....\" in hash table\n");
        printf( "\"%s\" count:"
                "text:      %ld\n", word, text_counter);

        return false;
    }

    return text_counter == (search_res -> counter);
}
