#ifndef _LIST_DATA_STRUCTURE_H__
#define _LIST_DATA_STRUCTURE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//#include "hash_table.h"
#include "params.h"

struct hashtable_elem_t {
    char buffer[STRING_SIZE];
    size_t hash;
    size_t counter;

    char aligning[16];
};

typedef hashtable_elem_t lst_data_t;
typedef int32_t lst_index_t;

const lst_index_t realloc_coeff = 2;

struct list_t
{
    lst_data_t*  data;
    lst_index_t* next;
    lst_index_t* prev;

    lst_index_t free;
    lst_index_t capacity;
};

typedef lst_data_t* (ListSearchFunc)(list_t* lst, char* key);

// LIST FUNCTIONS -----------------------------------------------------------------------------------------------------------------------------
list_t list_ctor();
int list_dtor(list_t* lst);
int list_verify(list_t* lst);

inline lst_data_t* list_next(list_t* lst, lst_index_t i);
inline lst_data_t* list_prev(list_t* lst, lst_index_t i);
inline lst_data_t* list_get_front(list_t* lst);
inline lst_data_t* list_get_back(list_t* lst);

int list_push_back (list_t* lst, lst_data_t elem);

extern "C" lst_data_t* list_search_asm(list_t* lst, char* key);
extern "C" lst_data_t* list_search_asm_preload(list_t* lst, char* key);
extern "C" lst_data_t* list_search_asm_opt(list_t* lst, char* key);

lst_data_t* list_search(list_t* lst, char key[STRING_SIZE]);
lst_data_t* list_search_opt(list_t* lst, char key[STRING_SIZE]);
lst_data_t* list_search_AVX(list_t* lst, char key[STRING_SIZE]);

int list_insert_after(list_t* lst, lst_index_t i, lst_data_t elem);
int list_expand(list_t* lst);
int list_erase(list_t* lst, lst_index_t i);

// LIST DUMP ----------------------------------------------------------------------------------------------------------------------------------
FILE* prepare_to_dump();

int list_dump(list_t* lst, FILE* stream);
int make_dot_file(list_t* lst, FILE* fp);
int dump_to_html(list_t* lst, char* png_file_name, FILE* html_stream);

#endif
