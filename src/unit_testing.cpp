#include "unit_testing.h"
#include "hash_table.h"
#include "params.h"

const Test_Hashtable_Funcs_Set Hashtable_Funcs_Set[] = {
    {&insert, &insert_preload},
    {}
};

const size_t NUM_OF_SETS_FOR_TEST = sizeof(Hashtable_Funcs_Set) / sizeof(Hashtable_Funcs_Set[0]);

const size_t benchmark_index = 0;
const char dump_file_name[] = "run_results.txt";

void run_tests(size_t n_tests) {

    FILE* fp_for_insert = fopen(processed_text, "r");
    if (input_fp == NULL) { perror("Stream for insert test not opened"); }
    FILE* fp_for_search = fopen(test_for_search, "r");
    if (input_fp == NULL) { perror("Stream for search test not opened"); }

    printf("Number of functions to test: %zu\n", NUM_OF_SETS_FOR_TEST);

    uint64_t test_results[NUM_OF_SETS_FOR_TEST] = {};
    uint64_t normilized_test_results[NUM_OF_SETS_FOR_TEST] = {};
    uint64_t temp_res = 0;

    uint64_t start = 0;
    uint64_t end   = 0;
    size_t counter = 0;
    size_t time_counter = 0;

    hashtable_t htbl = init();

    for (size_t n = 0; n < NUM_OF_SETS_FOR_TEST; n++) {

        htbl.insert = Hashtable_Funcs_Set[n].insert;
        htbl.search = Hashtable_Funcs_Set[n].search;

        while (fgets(buffer, STRING_SIZE, fp_for_insert)) {
            asm volatile ("rdtscp" : "=A" (start));
            htbl.insert(&htbl, buffer);
            asm volatile ("rdtscp" : "=A" (end));
            time_counter += end - start;
            counter++;
        }

        printf( "test number: %ld \n"
                "insert:\n"
                "ticks: %ld\n"
                "counter = %ld\n", n, time_counter counter);

        time_counter = 0;
        counter = 0;
        while (fgets(buffer, STRING_SIZE, fp_for_search)) {
            asm volatile ("rdtscp" : "=A" (start));
            htbl.search(&htbl, buffer);
            asm volatile ("rdtscp" : "=A" (end));
            time_counter += end - start;
            counter++;
        }

        printf( "search:\n"
                "ticks: %ld\n"
                "counter = %ld\n", n, time_counter counter);

        fseek(fp_for_insert, 0, SEEK_SET);
        fseek(fp_for_search, 0, SEEK_SET);
    }

    printf("-----Testing results-----\n");
    printf("Number of processor cycles per function:\n");
    for (size_t i = 0; i < NUM_OF_SETS_FOR_TEST; i++) {
        printf("%zu: %lu\n", i, test_results[i]);
    }

    printf("Normilized values:\n");
    double benchmark_elem = static_cast<double>(test_results[benchmark_index]);
    for (size_t i = 0; i < NUM_OF_SETS_FOR_TEST; i++) {
        printf("%zu: %lf\n", i, benchmark_elem / test_results[i]);
    }

    file_dump(test_results);
}

void file_dump(uint64_t* test_results) {
    assert(test_results);

    char results_string[BUFSIZ] = "";
    size_t written = uint64_array_to_string(results_string, BUFSIZ, test_results, NUM_OF_SETS_FOR_TEST);

    char file_name[64] = "";
    snprintf(file_name, 64, "dump/%s", dump_file_name);

    FILE* fp = fopen(file_name, "a+");
    if (fp == NULL) {
        fprintf(stderr, "Could not open file for writing %s in %s in %s:%d", dump_file_name, __func__, __FILE__, __LINE__);
        return;
    }

    fwrite(results_string, written, sizeof(char), fp);

    if (not fclose(fp)) {
        fprintf(stderr, "Could not close file after writing %s in %s in %s:%d", dump_file_name, __func__, __FILE__, __LINE__);
    }
}
