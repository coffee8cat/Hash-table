#include "hash_table.h"

size_t n_buckets = 13;
size_t bucket_size = 20;


hashtable_t init() {
    hashtable_t htbl = {};

    htbl.buckets = (bucket_t*)calloc(n_buckets, sizeof(bucket_t));
    if (htbl.buckets == NULL) { perror("Failed to allocate buckets for hashtable"); abort(); }

    for (size_t i = 0; i < n_buckets; i++) {
        htbl.buckets[i].data = list_ctor();
    }

    return htbl;
}

size_t djb2_hash(const char* string) {

    size_t hash = 5381;
    char c = 0;

    while (c = *(string++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c

    return hash;
}

int search(hashtable_t* htbl, const char* word) {
    assert(htbl);
    assert(word);

    size_t bucket_index = djb2_hash(word) % n_buckets;
    printf("bucket index for search of \"%16s\" = %ld\n", word, bucket_index);
    return list_search(&(htbl -> buckets[bucket_index].data), word);
}

int insert(hashtable_t* htbl, const char* word) {
    assert(htbl);
    assert(word);

    size_t index = djb2_hash(word) % n_buckets;
    printf("insert index for \"%16s\" = %ld\n", word, index);

    list_t* temp_lst = &(htbl -> buckets[index].data);

    // check already stored duplicates
    if (list_search(temp_lst, word) != 0) {
        return 1;
    }

    list_push_back(temp_lst, word);

    return 0;
}

void dstr(hashtable_t* htbl) {
    assert(htbl);

    for (size_t i = 0; i < n_buckets; i++) {
        list_dtor(&(htbl -> buckets[i].data));
    }

    memset(htbl, 0, sizeof(hashtable_t));
}
