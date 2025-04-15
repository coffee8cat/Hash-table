#include "my_list.h"

int main() {
    FILE* html_stream = prepare_to_dump();
    if (html_stream == NULL) { perror("html stream not opened"); }

    list_t lst = list_ctor();
    list_dump(&lst, html_stream);

    //list_insert_after(&lst, 0, "abcd");
    list_push_back(&lst, "abcd");
    list_dump(&lst, html_stream);

    list_push_back(&lst, "ert");
    list_dump(&lst, html_stream);

    list_push_back(&lst, "xyz");
    list_dump(&lst, html_stream);

    lst_index_t index = list_search(&lst, "ert");
    printf("index = %d\n", index);

    list_dtor(&lst);
    if (fclose(html_stream) != 0) { perror("html stream not closed"); }

    return 0;
}
