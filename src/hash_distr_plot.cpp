#include "hash_table.h"
#include "params.h"


void get_spectrum(hashtable_t* htbl) {
    assert(htbl);

    const size_t htbl_size = htbl -> size;

    uint16_t buckets_sizes[htbl_size] = {};
    size_t bucket_size_counters[size_distr_analysis_width] = {};
    size_t out_of_range_counter = 0;

    collect_distr_data(htbl, buckets_sizes, bucket_size_counters, &out_of_range_counter);
    save_distr_plot_descr(buckets_sizes, bucket_size_counters);

    // call gnu plot
    int result = system("gnuplot plot.gp");
    if (result != 0) {
        printf("Ошибка при запуске Gnuplot. Код возврата: %d\n", result);
    } else {
        printf("График успешно построен и сохранён.\n");
    }

    // print size distr res
    printf("--- Hashtable spectrum ---\n\n");
    printf("size | counter\n");
    for (size_t i = 0; i < size_distr_analysis_width; i++) {
        printf("%4ld | %7ld\n", i, bucket_size_counters[i]);
    }
    printf("Out of range (>%ld): %ld\n", size_distr_analysis_width, out_of_range_counter);
}

void collect_distr_data(hashtable_t* htbl, uint16_t* buckets_sizes, size_t* bucket_size_counters, size_t* out_of_range_counter) {
    assert(htbl);
    assert(buckets_sizes);
    assert(bucket_size_counters);
    assert(out_of_range_counter);

    const size_t htbl_size = htbl -> size;
    uint16_t temp_size = 0;

    for (size_t i = 0; i < htbl_size; i++) {
        temp_size = (uint16_t)htbl -> buckets[i].size;
        buckets_sizes[i] = temp_size;
        if (temp_size < size_distr_analysis_width) {
            (bucket_size_counters[temp_size])++;
        }
        else {
            (*out_of_range_counter)++;
        }
    }
}

void save_distr_plot_descr(uint16_t* buckets_sizes, size_t* bucket_size_counters) {
    assert(buckets_sizes);
    assert(bucket_size_counters);

    FILE* fp = fopen(fname_buckets_distr_data, "w");
    if (fp) {
        fprintf(fp, "Bucket,Count\n");
        for (int i = 0; i < BUCKETS_NUM; i++) {
            fprintf(fp, "%d,%d\n", i, buckets_sizes[i]);
        }

        fclose(fp);
        printf("Распределение сохранено в %s\n", fname_buckets_distr_data);
    }
    else {
        perror("fopen");
    }

    FILE* sizes_fp = fopen(fname_size_distr_data, "w");
    if (sizes_fp) {
        fprintf(fp, "Size,Count\n");
        for (int i = 0; i < size_distr_analysis_width; i++) {
            fprintf(fp, "%d,%ld\n", i, bucket_size_counters[i]);
        }

        fclose(fp);
        printf("Распределение сохранено в %s\n", fname_size_distr_data);
    }
    else {
        perror("fopen");
    }

    // prepare plot.gp descr for gnu plot
    FILE *gnuplot_pipe = fopen("plot.gp", "w");
    if (gnuplot_pipe) {
        fprintf(gnuplot_pipe, "set datafile separator comma\n"
                "set terminal pngcairo size 800,600 enhanced font \'Verdana,10\'\n"
                "set output \'%s\'\n\n"
                "set title \"CRC32 Hash Bucket Distribution\"\n"
                "set xlabel \"Bucket\"\n"
                "set ylabel \"Count\"\n"
                "set grid\n"
                "set boxwidth 0.8 relative\n"
                "set style fill solid\n\n"
                "plot \'data/bucket_distribution.csv\' using 1:2 skip 1 with boxes lc rgb \"#406090\" notitle\n\n\n"

                "set datafile separator comma\n"
                "set terminal pngcairo size 800,600 enhanced font \'Verdana,10\'\n"
                "set output \'%s\'\n\n"
                "set title \"CRC32 Hash Sizes Distribution\"\n"
                "set xlabel \"Size\"\n"
                "set ylabel \"Count\"\n"
                "set grid\n"
                "set boxwidth 0.8 relative\n"
                "set style fill solid\n\n"
                "plot \'data/sizes_distribution.csv\' using 1:2 skip 1 with boxes lc rgb \"#406090\" notitle\n",
                fname_buckets_distr_plot, fname_size_distr_plot);

        fclose(gnuplot_pipe);
    }
    else {
        perror("popen gnuplot");
    }
}
