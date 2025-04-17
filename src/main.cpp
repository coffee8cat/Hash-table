#include "my_list.h"
#include "hash_table.h"
#include "text_processing.h"

#include <cstdint>
#include <x86intrin.h>

int main() {

    const char processed_text[] = "text/War-and-Peace-processed.txt";
    const char test_for_search[] = "text/War-and-Peace-test.txt";

    /*
    process_raw_text();
    prepare_test_for_search();
    */

    FILE* html_stream = prepare_to_dump();
    if (html_stream == NULL) { perror("html stream not opened"); }

    hashtable_t htbl = init();

    FILE* input_fp = fopen(processed_text, "r");
    const size_t buffer_size = 16;
    char buffer[buffer_size] = {};


    uint64_t start = __rdtsc();
    size_t counter = 0;
    while (fgets(buffer, buffer_size, input_fp)) {
        insert(&htbl, buffer);
        counter++;
    }

    uint64_t end = __rdtsc();

    printf("insert test\n");
    printf("ticks: %ld\n", end - start);
    printf("counter = %ld\n", counter);

    if (fclose(input_fp) != 0)      { perror("input stream not closed"); }

//===============================================================================================================================

    input_fp = fopen(test_for_search, "r");

    start = __rdtsc();
    counter = 0;

    while (fgets(buffer, buffer_size, input_fp)) {
        search(&htbl, buffer);
        counter++;
    }

    end = __rdtsc();

    printf("search test\n");
    printf("ticks: %ld\n", end - start);
    printf("counter = %ld\n", counter);

    char test_buf[16] = "you";
    hashtable_elem_t* search_res = search(&htbl, test_buf);
    if (search_res) {
        printf("search \"you\" result: [%16s], %ld", search_res -> buffer, search_res -> counter);
    }
    else { printf("no..."); }

    if (fclose(html_stream) != 0)   { perror("html stream not closed"); }
    if (fclose(input_fp) != 0)      { perror("input stream not closed"); }

    get_spectrum(&htbl);

    dstr(&htbl);

    return 0;
}
