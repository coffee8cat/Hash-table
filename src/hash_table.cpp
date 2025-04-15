#include "hash_table.h"

/*
size_t n_buckets = 13;
size_t bucket_size = 20;


hash_table_t* init() {
    hash_table_t* htbl = (hash_table_t*)calloc(1, sizeof(hash_table_t));
    if (!htbl) { perror("%s:%d", __FILE__, __LINE__); return NULL; }

    htbl -> buckets = (bucket_t*)calloc(n_buckets, sizeof(bucket_t));
    if (htbl -> buckets == NULL) { perror("%s:%d", __FILE__, __LINE__); return NULL; }

    for (size_t i = 0; i < n_buckets; i++) {
        htbl.buckets[i].data = list_ctor();
    }

    return htbl;
}

size_t hash(const char* string) {

    size_t hash = 5381;
    char c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; // hash * 33 + c

    return hash;
}

int search(hash_table_t* htbl, const char* string) {
    assert(htbl);
    assert(string);

    size_t index = hash(string) % n_buckets;
    htbl_elem_t* node = {};
    if ( (node = list_search(htbl -> buckets[index].data, string)) != NULL) {

    }
}

int insert(hash_table_t* htbl, const char* string) {
    assert(htbl);
    assert(string);

    size_t index = hash(string) % n_buckets;

    // check already stored duplicates
    if (list_search(htbl -> buckets[index].data, string) != NULL) {
        return 0;
    }
    list_insert_after(htbl -> buckets[index].data, 0, string);

    return 0;
}

int dstr();
*/
