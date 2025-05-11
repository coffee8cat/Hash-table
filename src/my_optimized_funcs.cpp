#include "my_optimized_funcs.h"

bool my_str32cmp(char word1[32], char word2[32]) {
    assert(word1);
    assert(word2);

    int mask = 0;
    __asm__ volatile (
        "vmovdqa (%1),     %%ymm0 \n\t"             // ymm0 = word1
        "vmovdqa (%2),     %%ymm1 \n\t"             // ymm1 = word2
        "vpcmpeqb  %%ymm1, %%ymm0, %%ymm0\n\t"      // ymm0 = cmpeq(ymm0, ymm1)
        "vpmovmskb %%ymm0, %0 \n\t"                 // mask = movemask(ymm0)
        : "=r"(mask)                                // Exit
        : "r"(word1), "r"(word2)                    // Entry
        : "ymm0", "ymm1"                            // Dstr
    );

    //__m256i word1_ymm = _mm256_load_si256((__m256i*)word1);
    //__m256i word2_ymm = _mm256_load_si256((__m256i*)word2);

    //__m256i cmp = _mm256_cmpeq_epi8(word1_ymm, word2_ymm);

    //int mask = _mm256_movemask_epi8(cmp);

    return mask == 0xFFFF;
}

