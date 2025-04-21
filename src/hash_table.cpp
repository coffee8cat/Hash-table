#include "hash_table.h"

size_t n_buckets = 2047;
size_t bucket_size = 20;


hashtable_t init() {
    hashtable_t htbl = {};

    htbl.buckets = (bucket_t*)calloc(n_buckets, sizeof(bucket_t));
    if (htbl.buckets == NULL) { perror("Failed to allocate buckets for hashtable"); abort(); }

    htbl.size = n_buckets;
    for (size_t i = 0; i < n_buckets; i++) {
        htbl.buckets[i].data = list_ctor();
    }

    return htbl;
}

void zero_after_null(char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (data[i] == 0) {
            for (size_t j = i + 1; j < size; j++) {
                data[j] = 0;
            }
            break;
        }
    }
}

hashtable_elem_t* search(hashtable_t* htbl, char word[16]) {
    assert(htbl);
    assert(word);

    size_t word_len = strlen(word);
    zero_after_null(word, 16);

    //size_t hash = crc32((uint8_t*)word, word_len);
    size_t hash = MurmurHash64A(word, word_len, 0x1234ABCD);
    size_t bucket_index = hash % n_buckets;

    //printf("bucket index for search of \"%16s\" = %ld\n", word, bucket_index);
    return list_search(&(htbl -> buckets[bucket_index].data), word);
}

int insert(hashtable_t* htbl, char word[16]) {
    assert(htbl);
    assert(word);

    size_t word_len = strlen(word);
    zero_after_null(word, 16);

    //size_t hash = crc32((uint8_t*)word, word_len);
    size_t hash = MurmurHash64A(word, word_len, 0x1234ABCD);
    size_t index = hash % n_buckets;

    //printf("insert index for \"%16s\" = %ld\n", word, index);

    bucket_t* temp_bucket = &(htbl -> buckets[index]);
    list_t* temp_lst = &(temp_bucket -> data);

    // check already stored duplicates
    if (list_search(temp_lst, word) != NULL) {
        return 1;
    }

    hashtable_elem_t word_wrap = {.hash = hash};
    strncpy(word_wrap.buffer, word, word_len);

    (temp_bucket -> size)++;
    list_push_back(temp_lst, word_wrap);

    return 0;
}

void destroy_hashtable(hashtable_t* htbl) {
    assert(htbl);

    for (size_t i = 0; i < n_buckets; i++) {
        list_dtor(&(htbl -> buckets[i].data));
    }

    memset(htbl, 0, sizeof(hashtable_t));
}

void get_spectrum(hashtable_t* htbl) {
    assert(htbl);

    const size_t htbl_size = htbl -> size;

    uint16_t buckets_sizes[htbl_size] = {};
    size_t bucket_size_counters[256] = {};

    size_t sum_of_bucket_size = 0;
    float avg_bucket_size = 0;
    size_t min_bucket_size = 0;
    uint16_t temp_size = 0;

    size_t out_of_range_counter = 0;

    for (size_t i = 0; i < htbl_size; i++) {
        temp_size = (uint16_t)htbl -> buckets[i].size;
        buckets_sizes[i] = temp_size;
        if (temp_size < 256) {
            (bucket_size_counters[temp_size])++;
        }
        else {
            out_of_range_counter++;
        }
    }

    FILE* fp = fopen("data/bucket_distribution.csv", "w");
    if (fp) {
        fprintf(fp, "Bucket,Count\n");
        for (int i = 0; i < n_buckets; i++) {
            fprintf(fp, "%d,%d\n", i, buckets_sizes[i]);
        }

        fclose(fp);
        printf("Распределение сохранено в data/bucket_distribution.csv\n");
    }
    else {
        perror("fopen");
    }

    FILE* sizes_fp = fopen("data/sizes_distribution.csv", "w");
    if (sizes_fp) {
        fprintf(fp, "Size,Count\n");
        for (int i = 0; i < 20; i++) {
            fprintf(fp, "%d,%d\n", i, bucket_size_counters[i]);
        }

        fclose(fp);
        printf("Распределение сохранено в data/sizes_distribution.csv\n");
    }
    else {
        perror("fopen");
    }

    FILE *gnuplot_pipe = fopen("plot.gp", "w");
    if (gnuplot_pipe) {
        fprintf(gnuplot_pipe, "set datafile separator comma\n"
                "set terminal pngcairo size 800,600 enhanced font \'Verdana,10\'\n"
                "set output \'murmur_bucket_distribution.png\'\n\n"
                "set title \"Murmur64A Hash Bucket Distribution\"\n"
                "set xlabel \"Bucket\"\n"
                "set ylabel \"Count\"\n"
                "set grid\n"
                "set boxwidth 0.8 relative\n"
                "set style fill solid\n\n"
                "plot \'data/bucket_distribution.csv\' using 1:2 skip 1 with boxes lc rgb \"#406090\" notitle\n\n\n"

                "set datafile separator comma\n"
                "set terminal pngcairo size 800,600 enhanced font \'Verdana,10\'\n"
                "set output \'murmur_sizes_distribution.png\'\n\n"
                "set title \"Murmur64A Hash Sizes Distribution\"\n"
                "set xlabel \"Size\"\n"
                "set ylabel \"Count\"\n"
                "set grid\n"
                "set boxwidth 0.8 relative\n"
                "set style fill solid\n\n"
                "plot \'data/sizes_distribution.csv\' using 1:2 skip 1 with boxes lc rgb \"#406090\" notitle\n");

        fclose(gnuplot_pipe);
    }
    else {
        perror("popen gnuplot");
    }

    int result = system("gnuplot plot.gp");
    if (result != 0) {
        printf("Ошибка при запуске Gnuplot. Код возврата: %d\n", result);
    } else {
        printf("График успешно построен и сохранён.\n");
    }

    printf("--- Hashtable spectrum ---\n\n");
    printf("size | counter\n");
    for (size_t i = 0; i < 16; i++) {
        printf("%4ld | %7ld\n", i, bucket_size_counters[i]);
    }
    printf("Out of range (>256): %ld\n", out_of_range_counter);
}
