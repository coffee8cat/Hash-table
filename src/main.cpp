#include "my_list.h"
#include "hash_table.h"

int main() {
    FILE* html_stream = prepare_to_dump();
    if (html_stream == NULL) { perror("html stream not opened"); }

    hashtable_t htbl = init();
    insert(&htbl, "determine");
    insert(&htbl, "allocate");
    insert(&htbl, "what for?");

    printf("search res = %d\n", search(&htbl, "allocate"));


    if (fclose(html_stream) != 0) { perror("html stream not closed"); }

    return 0;
}
