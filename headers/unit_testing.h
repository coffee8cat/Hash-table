#ifndef _UNIT_TESTING_FOR_HASHTABLE_H__
#define _UNIT_TESTING_FOR_HASHTABLE_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <cstdint>
#include <x86intrin.h>
#include <stdint.h>
#include <sched.h>      // Для cpu_set_t, CPU_SET, CPU_ZERO, sched_setaffinity
#include <unistd.h>

#include "hash_table.h"
#include "params.h"

static __inline__ uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ (
        "rdtsc"
        : "=a" (lo), "=d" (hi)
    );
    return ((uint64_t)hi << 32) | lo;
}

typedef struct {
    InsertFunc*     insert;
    SearchFunc*     search;
    ListSearchFunc* list_search;
    HashFunc*       hash_func;
} Test_Hashtable_Funcs_Set;

void pin_to_core(int core_id);
void run_tests(size_t num_of_tests);
bool check_word (hashtable_t* htbl, const char* word, FILE* html_stream);

#endif
