/*
list_t init_list() {
    size_t list_init_size = 40;
    size_t lst_size = n_buckets * list_init_size;
    list_t lst = {};

    lst.data = (lst_data_t*) calloc(lst_size,  sizeof(lst_data_t));
    lst.next = (lst_index_t*)calloc(lst_size,  sizeof(lst_index_t));
    lst.prev = (lst_index_t*)calloc(lst_size,  sizeof(lst_index_t));
    lst.free = (lst_index_t*)calloc(n_buckets, sizeof(lst_index_t));

    for (size_t i = 0; i < n_buckets; i++) {
        lst.free[i] = i * list_init_size + 1;
        lst.next[i * list_init_size] = 0;
        lst.prev[i * list_init_size] = 0;
        for (size_t j = 1; j < list_init_size - 1; j++) {
            lst.next[i * list_init_size + j] = i * list_init_size + j + 1;
            lst.prev[i * list_init_size + j] = -1;
        }

        lst.prev[(i + 1) * list_init_size - 1] = -1;
        lst.next[(i + 1) * list_init_size - 1] = 0;
    }
}
*/
