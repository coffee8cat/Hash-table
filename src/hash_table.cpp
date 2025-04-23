#include "hash_table.h"
#include "params.h"

hashtable_t init() {
    hashtable_t htbl = {};

    htbl.buckets = (bucket_t*)calloc(BUCKETS_NUM, sizeof(bucket_t));
    if (htbl.buckets == NULL) { perror("Failed to allocate buckets for hashtable"); abort(); }

    htbl.size = BUCKETS_NUM;
    for (size_t i = 0; i < BUCKETS_NUM; i++) {
        htbl.buckets[i].data = list_ctor();
    }

    return htbl;
}

void destroy_hashtable(hashtable_t* htbl) {
    assert(htbl);

    for (size_t i = 0; i < BUCKETS_NUM; i++) {
        list_dtor(&(htbl -> buckets[i].data));
    }

    memset(htbl, 0, sizeof(hashtable_t));
}

hashtable_elem_t* search(hashtable_t* htbl, char word[STRING_SIZE]) {
    assert(htbl);
    assert(word);

    size_t word_len = strlen(word);
    memset(word + word_len, 0, STRING_SIZE - word_len);

    //size_t hash = crc32((uint8_t*)word, word_len);
    size_t hash = MurmurHash64A(word, word_len, 0x1234ABCD);
    size_t bucket_index = hash % BUCKETS_NUM;

    //printf("bucket index for search of \"%16s\" = %ld\n", word, bucket_index);
    return list_search(&(htbl -> buckets[bucket_index].data), word);
}


hashtable_elem_t* search_AVX(hashtable_t* htbl, char word[STRING_SIZE]) {
    assert(htbl);
    assert(word);

    size_t word_len = strlen(word);
    memset(word + word_len, 0, STRING_SIZE - word_len);

    //size_t hash = crc32((uint8_t*)word, word_len);
    size_t hash = MurmurHash64A(word, word_len, 0x1234ABCD);
    size_t bucket_index = hash % BUCKETS_NUM;

    //printf("bucket index for search of \"%16s\" = %ld\n", word, bucket_index);
    return list_search_AVX(&(htbl -> buckets[bucket_index].data), word);
}

hashtable_elem_t* search_preload(hashtable_t* htbl, char word[STRING_SIZE]) {
    assert(htbl);
    assert(word);

    size_t word_len = strlen(word);
    memset(word + word_len, 0, STRING_SIZE - word_len);

    //size_t hash = crc32((uint8_t*)word, word_len);
    size_t hash = MurmurHash64A(word, word_len, 0x1234ABCD);
    size_t bucket_index = hash % BUCKETS_NUM;

    //printf("bucket index for search of \"%16s\" = %ld\n", word, bucket_index);
    return list_search_asm_preload(&(htbl -> buckets[bucket_index].data), word);
}

void insert(hashtable_t* htbl, char word[STRING_SIZE]) {
    assert(htbl);
    assert(word);

    size_t word_len = strlen(word);
    memset(word + word_len, 0, STRING_SIZE - word_len);

    //size_t hash = crc32((uint8_t*)word, word_len);
    size_t hash = MurmurHash64A(word, word_len, 0x1234ABCD);
    size_t index = hash % BUCKETS_NUM;

    //printf("insert index for \"%16s\" = %ld\n", word, index);

    bucket_t* temp_bucket = &(htbl -> buckets[index]);
    list_t* temp_lst = &(temp_bucket -> data);

    // check already stored duplicates
    lst_data_t* search_res_lst = list_search(temp_lst, word);
    if (search_res_lst != NULL) {
        (search_res_lst -> counter)++;
        return;
    }

    hashtable_elem_t word_wrap = {.hash = hash, .counter = 1};
    memcpy(word_wrap.buffer, word, word_len);

    (temp_bucket -> size)++;
    list_push_back(temp_lst, word_wrap);

    return;
}


void insert_AVX(hashtable_t* htbl, char word[STRING_SIZE]) {
    assert(htbl);
    assert(word);

    size_t word_len = strlen(word);
    memset(word + word_len, 0, STRING_SIZE - word_len);

    //size_t hash = crc32((uint8_t*)word, word_len);
    size_t hash = MurmurHash64A(word, word_len, 0x1234ABCD);
    size_t index = hash % BUCKETS_NUM;

    //printf("insert index for \"%16s\" = %ld\n", word, index);

    bucket_t* temp_bucket = &(htbl -> buckets[index]);
    list_t* temp_lst = &(temp_bucket -> data);

    // check already stored duplicates
    lst_data_t* search_res_lst = list_search_AVX(temp_lst, word);
    if (search_res_lst != NULL) {
        (search_res_lst -> counter)++;
        return;
    }

    hashtable_elem_t word_wrap = {.hash = hash, .counter = 1};
    memcpy(word_wrap.buffer, word, word_len);

    (temp_bucket -> size)++;
    list_push_back(temp_lst, word_wrap);

    return;
}

void insert_preload(hashtable_t* htbl, char word[STRING_SIZE]) {
    assert(htbl);
    assert(word);

    size_t word_len = strlen(word);
    memset(word + word_len, 0, STRING_SIZE - word_len);

    //size_t hash = crc32((uint8_t*)word, word_len);
    size_t hash = MurmurHash64A(word, word_len, 0x1234ABCD);
    size_t index = hash % BUCKETS_NUM;

    //printf("insert index for \"%16s\" = %ld\n", word, index);

    bucket_t* temp_bucket = &(htbl -> buckets[index]);
    list_t* temp_lst = &(temp_bucket -> data);

    // check already stored duplicates
    lst_data_t* search_res_lst = list_search_asm_preload(temp_lst, word);
    if (search_res_lst != NULL) {
        (search_res_lst -> counter)++;
        return;
    }

    hashtable_elem_t word_wrap = {.hash = hash, .counter = 1};
    memcpy(word_wrap.buffer, word, word_len);

    (temp_bucket -> size)++;
    list_push_back(temp_lst, word_wrap);

    return;
}
