#include "my_optimized_funcs.h"

bool my_str16cmp(char word1[16], char word2[16]) {
    assert(word1);
    assert(word2);

    __m128i word1_xmm = _mm_loadu_si128((__m128i*)word1);
    __m128i word2_xmm = _mm_loadu_si128((__m128i*)word2);

    __m128i cmp = _mm_cmpeq_epi8(word1_xmm, word2_xmm);

    int mask = _mm_movemask_epi8(cmp);

    return mask == 0xFFFF;
}
