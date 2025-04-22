#ifndef _UNIT_TESTING_FOR_HASHTABLE_H__
#define _UNIT_TESTING_FOR_HASHTABLE_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <cstdint>
#include <x86intrin.h>

typedef struct {
    InsertFunc* insert;
    SearchFunc* search;
} Test_Hashtable_Funcs_Set;

#endif
