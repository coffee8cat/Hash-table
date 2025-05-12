#include "hash_table.h"

static uint32_t crc32_table[256] = {};

void init_crc32_table() {
    uint32_t crc;
    for (int i = 0; i < 256; i++) {
        crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
        crc32_table[i] = crc;
    }
}

uint32_t crc32(const char* data) {
    size_t length = strlen(data);
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++) {
        uint8_t index = (crc ^ data[i]) & 0xFF;
        crc = (crc >> 8) ^ crc32_table[index];
    }
    return crc ^ 0xFFFFFFFF;
}

uint32_t crc32_16(const char data[STRING_SIZE]) {
    uint32_t crc = 0xFFFFFFFF;

    uint64_t part1 = *(const uint64_t*)(data);
    crc = _mm_crc32_u64(crc, part1);

    uint64_t part2 = *(const uint64_t*)(data + 8);
    crc = _mm_crc32_u64(crc, part2);

    return crc ^ 0xFFFFFFFF;
}


uint32_t crc32_32(const char data[32]) {
    uint32_t crc = 0xFFFFFFFF;

    uint64_t part1 = *(const uint64_t*)(data);
    crc = _mm_crc32_u64(crc, part1);

    uint64_t part2 = *(const uint64_t*)(data + 8);
    crc = _mm_crc32_u64(crc, part2);

    uint64_t part3 = *(const uint64_t*)(data + 16);
    crc = _mm_crc32_u64(crc, part3);

    uint64_t part4 = *(const uint64_t*)(data + 24);
    crc = _mm_crc32_u64(crc, part4);

    return crc ^ 0xFFFFFFFF;
}

uint64_t MurmurHash64A(const void * key, size_t len, uint64_t seed) {
    assert(key);

    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;

    uint64_t h = seed ^ (len * m);

    const uint64_t *data = (const uint64_t *)key;
    const uint64_t *end = data + (len / 8);

    while (data != end) {
        uint64_t k = *data++;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const unsigned char* data2 = (const unsigned char*)data;
    switch (len & 7) {
        case 7: h ^= (uint64_t)(data2[6]) << 48;
        case 6: h ^= (uint64_t)(data2[5]) << 40;
        case 5: h ^= (uint64_t)(data2[4]) << 32;
        case 4: h ^= (uint64_t)(data2[3]) << 24;
        case 3: h ^= (uint64_t)(data2[2]) << 16;
        case 2: h ^= (uint64_t)(data2[1]) << 8;
        case 1: h ^= (uint64_t)(data2[0]);
                h *= m;
    }

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

size_t djb2_hash(const char* string, size_t string_len) {
    assert(string);

    size_t hash = 5381;
    char c = 0;

    for (size_t i = 0; i < string_len; i++) {
        c = *(string++);
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}
