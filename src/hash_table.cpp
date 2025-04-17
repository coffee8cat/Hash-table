#include "hash_table.h"

size_t n_buckets = 1023;
size_t bucket_size = 20;


hashtable_t init() {
    hashtable_t htbl = {};

    htbl.buckets = (bucket_t*)calloc(n_buckets, sizeof(bucket_t));
    if (htbl.buckets == NULL) { perror("Failed to allocate buckets for hashtable"); abort(); }

    htbl.size = n_buckets;
    for (size_t i = 0; i < n_buckets; i++) {
        htbl.buckets[i].data = list_ctor();
    }

    return htbl;
}

uint64_t MurmurHash64A(const void *key, size_t len, uint64_t seed) {
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;

    uint64_t h = seed ^ (len * m);

    const uint64_t *data = (const uint64_t *)key;
    const uint64_t *end = data + (len / 8);

    while (data != end) {
        uint64_t k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char *data2 = (const unsigned char *)data;
    switch (len & 7) {
        case 7: h ^= (uint64_t)(data2[6]) << 48;
        case 6: h ^= (uint64_t)(data2[5]) << 40;
        case 5: h ^= (uint64_t)(data2[4]) << 32;
        case 4: h ^= (uint64_t)(data2[3]) << 24;
        case 3: h ^= (uint64_t)(data2[2]) << 16;
        case 2: h ^= (uint64_t)(data2[1]) << 8;
        case 1: h ^= (uint64_t)(data2[0]);
                h *= m;
    }

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

size_t djb2_hash(const char* string, size_t string_len) {

    size_t hash = 5381;
    char c = 0;

    for (size_t i = 0; i < string_len; i++) {
        c = *(string++);
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}

hashtable_elem_t* search(hashtable_t* htbl, const char* word) {
    assert(htbl);
    assert(word);

    size_t word_len = strlen(word);

    size_t hash = MurmurHash64A(word, word_len, 0x1234ABCD);
    size_t bucket_index = hash % n_buckets;

    //printf("bucket index for search of \"%16s\" = %ld\n", word, bucket_index);
    return list_search_asm(&(htbl -> buckets[bucket_index].data), hash);
}

hashtable_elem_t* search_str(hashtable_t* htbl, const char* word) {
    assert(htbl);
    assert(word);

    size_t word_len = strlen(word);

    size_t hash = MurmurHash64A(word, word_len, 0x1234ABCD);
    size_t bucket_index = hash % n_buckets;

    //printf("bucket index for search of \"%16s\" = %ld\n", word, bucket_index);
    return list_search_asm(&(htbl -> buckets[bucket_index].data), hash);
}

int insert(hashtable_t* htbl, const char* word) {
    assert(htbl);
    assert(word);

    size_t word_len = strlen(word);

    size_t hash = MurmurHash64A(word, word_len, 0x1234ABCD);
    size_t index = hash % n_buckets;

    //printf("insert index for \"%16s\" = %ld\n", word, index);

    bucket_t* temp_bucket = &(htbl -> buckets[index]);
    list_t* temp_lst = &(temp_bucket -> data);

    // check already stored duplicates
    if (list_search_asm(temp_lst, hash) != NULL) {
        return 1;
    }

    hashtable_elem_t word_wrap = {.hash = hash};
    strncpy(word_wrap.buffer, word, word_len);

    (temp_bucket -> size)++;
    list_push_back(temp_lst, word_wrap);

    return 0;
}

void dstr(hashtable_t* htbl) {
    assert(htbl);

    for (size_t i = 0; i < n_buckets; i++) {
        list_dtor(&(htbl -> buckets[i].data));
    }

    memset(htbl, 0, sizeof(hashtable_t));
}

void get_spectrum(hashtable_t* htbl) {
    assert(htbl);

    size_t bucket_size_counters[128] = {};

    size_t sum_of_bucket_size = 0;
    float avg_bucket_size = 0;
    size_t min_bucket_size = 0;
    size_t temp_size = 0;

    const size_t htbl_size = htbl -> size;
    for (size_t i = 0; i < htbl_size; i++) {
        temp_size = htbl -> buckets[i].size;
        (bucket_size_counters[temp_size])++;
    }

    printf("--- Hashtable spectrum ---\n\n");
    printf("size | counter\n");
    for (size_t i = 0; i < 128; i++) {
        printf("%4ld | %7ld\n", i, bucket_size_counters[i]);
    }
}

int check_collisions(hashtable_t* htbl, FILE* input_fp) {
    assert(htbl);
    assert(input_fp);




}
