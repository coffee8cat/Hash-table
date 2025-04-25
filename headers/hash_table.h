#ifndef _HASH_TABLE_H__
#define _HASH_TABLE_H__

#include <cstdint>
#include <x86intrin.h>

#include "my_list.h"
#include "params.h"

struct bucket_t {
    list_t data;
    int32_t size;
};

typedef struct hashtable_t hashtable_t;

typedef void              (InsertFunc)(hashtable_t*, char[STRING_SIZE]);
typedef hashtable_elem_t* (SearchFunc)(hashtable_t*, char[STRING_SIZE]);
typedef uint32_t          (HashFunc)(const char*);

struct hashtable_t {
    bucket_t* buckets;
    size_t size;

    InsertFunc*     insert;
    SearchFunc*     search;
    ListSearchFunc* list_search;
    HashFunc*       hash_func;
};

hashtable_t init();
void destroy_hashtable(hashtable_t* htbl);

hashtable_elem_t* search        (hashtable_t* htbl, char word[STRING_SIZE]);
//hashtable_elem_t* search_AVX    (hashtable_t* htbl, char word[STRING_SIZE]);
//hashtable_elem_t* search_preload(hashtable_t* htbl, char word[STRING_SIZE]);
//hashtable_elem_t* search_full_opt(hashtable_t* htbl, char word[STRING_SIZE]);

void insert         (hashtable_t* htbl, char word[STRING_SIZE]);
//void insert_AVX     (hashtable_t* htbl, char word[STRING_SIZE]);
//void insert_preload (hashtable_t* htbl, char word[STRING_SIZE]);
//void insert_full_opt(hashtable_t* htbl, char word[STRING_SIZE]);

void init_crc32_table();

HashFunc crc32;
HashFunc crc32_16;

uint64_t MurmurHash64A(const void *key, size_t len, uint64_t seed);
size_t djb2_hash(const char* string, size_t string_len);

void get_spectrum(hashtable_t* htbl);
void collect_distr_data(hashtable_t* htbl, uint16_t* buckets_sizes, size_t* bucket_size_counters, size_t* out_of_range_counter);
void save_distr_plot_descr(uint16_t* buckets_sizes, size_t* bucket_size_counters);

#endif
