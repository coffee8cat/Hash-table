#include "my_optimized_funcs.h"

bool my_str16cmp(char word1[STRING_SIZE], char word2[STRING_SIZE]) {
    assert(word1);
    assert(word2);

    int mask = 0;
    __asm__ volatile (
        "movdqu (%1),     %%xmm0 \n\t"    // xmm0 = word1
        "movdqu (%2),     %%xmm1 \n\t"    // xmm1 = word2
        "pcmpeqb  %%xmm1, %%xmm0 \n\t"    // xmm0 = cmpeq(xmm0, xmm1)
        "pmovmskb %%xmm0, %0 \n\t"        // mask = movemask(xmm0)
        : "=r"(mask)                      // Exit
        : "r"(word1), "r"(word2)          // Entry
        : "xmm0", "xmm1"                  // Dstr
    );
    /*
    __m128i word1_xmm = _mm_loadu_si128((__m128i*)word1);
    __m128i word2_xmm = _mm_loadu_si128((__m128i*)word2);

    __m128i cmp = _mm_cmpeq_epi8(word1_xmm, word2_xmm);

    int mask = _mm_movemask_epi8(cmp);
    */
    return mask == 0xFFFF;
}
