#include "text_processing.h"

const char raw_text[] = "text/War-and-Peace.txt";
const char processed_text[] = "text/War-and-Peace-processed.txt";
const char test_for_search[] = "text/War-and-Peace-test.txt";

void process_raw_text() {

    FILE* input_fp  = fopen(raw_text, "r");
    if (input_fp  == NULL) { perror("Could not open input file");  return; }
    FILE* output_fp = fopen(processed_text, "w");
    if (output_fp == NULL) { perror("Could not open output file"); return; }

    bool prev_ch_is_alpha = 0;
    bool temp_is_alpha = 0;
    char ch = 0;

    while ((ch = fgetc(input_fp)) != EOF) {
        if (temp_is_alpha = isalpha(ch)) {
            fputc(ch, output_fp);
        }
        else if (prev_ch_is_alpha) {
            fputc('\0', output_fp);
            fputc('\n', output_fp);
        }
        prev_ch_is_alpha = temp_is_alpha;
    }

    if (!fclose(input_fp))  { perror("Could not close input file"); };
    if (!fclose(output_fp)) { perror("Could not close output file"); };
}

void prepare_test_for_search() {

    FILE* input_fp  = fopen(processed_text, "r");
    if (input_fp  == NULL) { perror("Could not open input file");  return; }
    FILE* output_fp = fopen(test_for_search, "w");
    if (output_fp == NULL) { perror("Could not open output file"); return; }

    char ch = 0;
    char word_counter = 0;

    while ((ch = fgetc(input_fp)) != EOF) {
        if (ch == '\n') {
            word_counter++;
            if (word_counter == 4) {
                while ((ch = fgetc(input_fp)) != '\n' && ch != EOF) {
                    fputc(ch, output_fp);
                }
                fputc('\n', output_fp);
                word_counter = 0;
            }
        }
    }

    if (!fclose(input_fp))  { perror("Could not close input file"); };
    if (!fclose(output_fp)) { perror("Could not close output file"); };
}
