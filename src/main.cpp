#include "my_list.h"
#include "hash_table.h"
#include "text_processing.h"
#include "params.h"

#include <cstdint>
#include <x86intrin.h>

int main() {

    init_crc32_table();

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
    if (input_fp == NULL) { perror("Stream of processed text not opened"); }

    char buffer[STRING_SIZE] = {};


    uint64_t start = 0;
    uint64_t end   = 0;
    size_t counter = 0;
    size_t time_counter = 0;

    /*
    while (fgets(buffer, STRING_SIZE, input_fp)) {
        if (strncmp(buffer, "you", sizeof("you")) == 0)
            counter++;
    }
    printf("\"you\" counted %ld times\n\n", counter);
    counter = 0;
    */

    /*strncpy(buffer, "you", sizeof("you"));
    insert(&htbl, buffer);
    insert(&htbl, buffer);
    insert(&htbl, buffer);
    insert(&htbl, buffer);
    insert(&htbl, buffer);
    */
    while (fgets(buffer, STRING_SIZE, input_fp)) {
        asm volatile ("rdtscp" : "=A" (start));
        insert(&htbl, buffer);
        asm volatile ("rdtscp" : "=A" (end));
        time_counter += end - start;
        counter++;
    }

    printf("--- insert test --- \n");
    printf("ticks: %ld\n", time_counter);
    printf("counter = %ld\n", counter);

    fseek(input_fp, 0, SEEK_SET);

    time_counter = 0;
    counter      = 0;
    while (fgets(buffer, STRING_SIZE, input_fp)) {
        asm volatile ("rdtscp" : "=A" (start));
        insert_preload(&htbl, buffer);
        asm volatile ("rdtscp" : "=A" (end));
        time_counter += end - start;
        counter++;
    }

    printf("--- PRELOAD insert test --- \n");
    printf("ticks: %ld\n", time_counter);
    printf("counter = %ld\n", counter);

    if (fclose(input_fp) != 0) { perror("input stream not closed"); }

//===============================================================================================================================

    input_fp = fopen(test_for_search, "r");

    start = __rdtsc();
    counter = 0;

    while (fgets(buffer, STRING_SIZE, input_fp)) {
        search(&htbl, buffer);
        counter++;
    }

    end = __rdtsc();

    printf("search test\n");
    printf("ticks: %ld\n", end - start);
    printf("counter = %ld\n", counter);

    char test_buf[STRING_SIZE] = "you";
    hashtable_elem_t* search_res = search(&htbl, test_buf);
    if (search_res) {
        fprintf(stderr, "search \"you\" result: [%16s], %ld\n", search_res -> buffer, search_res -> counter);
    }
    else { fprintf(stderr, "Could not find \"yes\" in hash table\n"); }


    strcpy(test_buf,  "......");
    search_res = search(&htbl, test_buf);
    if (search_res) {
        fprintf(stderr, "search \".......\" result: [%16s], %ld\n", search_res -> buffer, search_res -> counter);
    }
    else { fprintf(stderr, "Could not find \".....\" in hash table\n"); }

    if (fclose(html_stream) != 0) { perror("html stream not closed"); }
    if (fclose(input_fp)    != 0) { perror("input stream not closed"); }

    get_spectrum(&htbl);

    strncpy(buffer, "you", sizeof("you"));
    size_t word_len = strlen(buffer);
    memset(buffer + word_len, 0, STRING_SIZE - word_len);

    //size_t hash = crc32((uint8_t*)word, word_len);
    size_t hash = MurmurHash64A(buffer, word_len, 0x1234ABCD);
    size_t bucket_index = hash % BUCKETS_NUM;
    list_dump(&(htbl.buckets[bucket_index].data), html_stream);

    destroy_hashtable(&htbl);

    return 0;
}
