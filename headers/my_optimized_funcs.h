#ifndef _MY_OPTIMIZED_FUNCTIONS_H__
#define _MY_OPTIMIZED_FUNCTIONS_H__

#include <cstdint>
#include <x86intrin.h>
#include <assert.h>

#include "params.h"

bool my_str16cmp(char word1[16], char word2[16]);
//bool my_str32cmp(char word1[32], char word2[32]);

#endif
