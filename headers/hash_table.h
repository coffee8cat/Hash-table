#ifndef _HASH_TABLE_H__
#define _HASH_TABLE_H__

#include "my_list.h"

struct bucket_t {
    list_t data;
    int32_t size;
};

struct hashtable_t {
    bucket_t* buckets;
    size_t size;
};


hashtable_t init();
int search(hashtable_t* htbl, const char* word);
int insert(hashtable_t* htbl, const char* word);
void dstr(hashtable_t* htbl);

size_t djb2_hash(const char* string);

//int getSize();
//int bucketSize();

#endif
