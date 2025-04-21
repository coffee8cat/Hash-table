#ifndef _MY_OPTIMIZED_FUNCTIONS_H__
#define _MY_OPTIMIZED_FUNCTIONS_H__

#include <cstdint>
#include <x86intrin.h>
#include <assert.h>

#include "params.h"

bool my_str16cmp(char word1[STRING_SIZE], char word2[STRING_SIZE]);

#endif
