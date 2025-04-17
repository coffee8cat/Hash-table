#ifndef _HASH_TABLE_H__
#define _HASH_TABLE_H__

#include "my_list.h"

#include <cstdint>
#include <x86intrin.h>

struct bucket_t {
    list_t data;
    int32_t size;
};

struct hashtable_t {
    bucket_t* buckets;
    size_t size;
};


hashtable_t init();
hashtable_elem_t* search(hashtable_t* htbl, const char* word);
int insert(hashtable_t* htbl, const char* word);
void dstr(hashtable_t* htbl);

uint64_t MurmurHash64A(const void *key, size_t len, uint64_t seed);
size_t djb2_hash(const char* string, size_t string_len);

void get_spectrum(hashtable_t* htbl);

#endif
