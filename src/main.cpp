#include "my_list.h"
#include "hash_table.h"
#include "text_processing.h"
#include "params.h"
#include "unit_testing.h"

#include <cstdint>
#include <x86intrin.h>

int main() {
    pin_to_core(0);
    init_crc32_table();

    FILE* html_stream = prepare_to_dump();
    if (html_stream == NULL)   { perror("html stream not opened"); return -1; }

    hashtable_t htbl = init();

    FILE* fp_for_insert = fopen(processed_text, "r");
    if (fp_for_insert == NULL) { perror("Stream of processed text not opened"); return -1; }

    char buffer[STRING_SIZE] = {};

    while (fgets(buffer, STRING_SIZE, fp_for_insert)) {
        insert(&htbl, buffer);
    }

    if (fclose(fp_for_insert) != 0) { perror("input stream not closed"); }

    FILE* fp_for_search = fopen(test_for_search, "r");
    if (fp_for_search == NULL) { perror("Stream of processed text not opened"); return -1; }

    while (fgets(buffer, STRING_SIZE, fp_for_search)) {
        search(&htbl, buffer);
    }

    get_spectrum(&htbl);
    check_word(&htbl, "you", html_stream);

    destroy_hashtable(&htbl);

    if (fclose(fp_for_search) != 0) { perror("input stream not closed"); }
    if (fclose(html_stream)   != 0) { perror("html stream not closed");  }

    run_tests(10);

    return 0;
}
