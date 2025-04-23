#include "my_list.h"
#include "my_optimized_funcs.h"

const lst_index_t lst_default_capacity = 20;


#define NEXT(i) lst -> next[i]
#define PREV(i) lst -> prev[i]

list_t list_ctor()
{
    list_t lst = {};

    lst.capacity = lst_default_capacity;
    lst.data = (lst_data_t*) calloc(lst.capacity, sizeof(lst_data_t));
    lst.next = (lst_index_t*)calloc(lst.capacity, sizeof(lst_index_t));
    lst.prev = (lst_index_t*)calloc(lst.capacity, sizeof(lst_index_t));

    if (!lst.data) { fprintf(stderr, "ERROR: Calloc failed: lst.data = NULL\n"); return lst; }
    if (!lst.next) { fprintf(stderr, "ERROR: Calloc failed: lst.next = NULL\n"); return lst; }
    if (!lst.prev) { fprintf(stderr, "ERROR: Calloc failed: lst.prev = NULL\n"); return lst; }

    lst.free    = 1;
    lst.next[0] = 0;
    lst.prev[0] = 0;

    for (lst_index_t i = 1; i < lst.capacity - 1; i++)
    {
        lst.next[i] = i + 1;
        lst.prev[i] = -1;
    }

    lst.prev[lst.capacity - 1] = -1;
    lst.next[lst.capacity - 1] = 0;

    return lst;
}

int list_dtor(list_t* lst)
{
    assert(lst);

    free(lst -> data); lst -> data = NULL;
    free(lst -> next); lst -> next = NULL;
    free(lst -> prev); lst -> prev = NULL;

    lst -> free = 0;
    lst -> capacity = 0;

    return 0;
}

int list_verify(list_t* lst)
{
    assert(lst);

    lst_index_t i = 0;
    lst_index_t j = 0;
    while (i != PREV(0) && j < lst -> capacity)
    {
        if (PREV(NEXT(i)) != i || NEXT(PREV(i)) != i)
        {
            fprintf(stderr, "VERIFICATION FAILED ON NODE %d\n", i);
            return i;
        }
        i = NEXT(i);
        j++;
    }

    i = NEXT(lst -> free);
    while (NEXT(i) != 0)
    {
        if (PREV(i) != -1)
        {
            fprintf(stderr, "INVALID PREV VALUE IN NODE %d: %d\n", i, PREV(i));
            return i;
        }
        i = NEXT(i);
    }

    return 0;
}

inline lst_data_t* list_next(list_t* lst, lst_index_t i)
{
    assert(lst);
    return &lst -> data[lst -> next[i]];
}

inline lst_data_t* list_prev(list_t* lst, lst_index_t i)
{
    assert(lst);
    return &lst -> data[lst -> prev[i]];
}

inline lst_data_t* list_get_front(list_t* lst)
{
    assert(lst);
    return &lst -> data[lst -> next[0]];
}

inline lst_data_t* list_get_back(list_t* lst)
{
    assert(lst);
    return &lst -> data[lst -> prev[0]];
}

int list_push_back(list_t* lst, lst_data_t elem)
{
    assert(lst);

    lst_index_t curr = lst -> free;
    lst -> free = NEXT(curr);

    lst -> data[curr] = elem;
    lst -> next[curr] = 0;

    lst -> next[lst -> prev[0]] = curr;
    lst -> prev[curr] = lst -> prev[0];
    lst -> prev[0] = curr;

    return 0;
}

int list_insert_after(list_t* lst, lst_index_t i, lst_data_t elem)
{
    assert(lst);

    if (lst -> prev[i] == -1)
    {
        fprintf(stderr, "ERROR: Cannot insert after free element\n");
        return -1;
    }
    if (lst -> free == 0)
    {
        list_expand(lst);
    }

    lst_index_t curr = lst -> free;
    lst -> free = NEXT(curr);

    //printf("got here\n");
    lst -> data[curr] = elem;

    lst -> prev[NEXT(i)] = curr;
    lst -> next[curr] = NEXT(i);

    lst -> prev[curr] = i;
    lst -> next[i] = curr;

    return 0;
}

lst_data_t* list_search(list_t* lst, char key[STRING_SIZE]) {
    assert(lst);

    lst_index_t curr_elem = PREV(0);

    while(curr_elem != 0) {
        lst_data_t temp_data = lst -> data[curr_elem];
        // next_elem = lst -> data[PREV(curr_elem)]
        //if (str16cmp(temp_data.buffer, key)) {
        if (strncmp(temp_data.buffer, key, 16) == 0) {
            return &(lst -> data[curr_elem]);
        }
        curr_elem = PREV(curr_elem);
        // prefetch(); ???
    }

    return NULL;
}

lst_data_t* list_search_AVX(list_t* lst, char key[STRING_SIZE]) {
    assert(lst);

    lst_index_t curr_elem = PREV(0);

    while(curr_elem != 0) {
        lst_data_t temp_data = lst -> data[curr_elem];
        // next_elem = lst -> data[PREV(curr_elem)]
        if (my_str16cmp(temp_data.buffer, key)) {
        //if (strncmp(temp_data.buffer, key, 16) == 0) {
            return &(lst -> data[curr_elem]);
        }
        curr_elem = PREV(curr_elem);
        // prefetch(); ???
    }

    return NULL;
}

int list_expand(list_t* lst)
{
    assert(lst);

    // ALLOCATING MEMORY FOR BIGGER LIST ==========================================================================================================

    lst_data_t* data_temp  = (lst_data_t*) realloc(lst -> data, realloc_coeff * lst -> capacity * sizeof(lst_data_t));
    lst_index_t* next_temp = (lst_index_t*)realloc(lst -> next, realloc_coeff * lst -> capacity * sizeof(lst_index_t));
    lst_index_t* prev_temp = (lst_index_t*)realloc(lst -> prev, realloc_coeff * lst -> capacity * sizeof(lst_index_t));

    if (!data_temp) { fprintf(stderr, "ERROR: Calloc failed: lst -> data = NULL\n"); return -1; }
    if (!next_temp) { fprintf(stderr, "ERROR: Calloc failed: lst -> next = NULL\n"); return -1; }
    if (!prev_temp) { fprintf(stderr, "ERROR: Calloc failed: lst -> prev = NULL\n"); return -1; }

    lst -> data = data_temp;
    lst -> next = next_temp;
    lst -> prev = prev_temp;

    // BONDING EXPANDED LIST ======================================================================================================================
    for (lst_index_t i = lst -> capacity; i < realloc_coeff * lst -> capacity; i++)
    {
        lst -> next[i] = (int)i + 1;
        lst -> prev[i] = -1;
    }
    lst -> next[realloc_coeff * lst -> capacity - 1] = 0;

    lst -> free = lst -> capacity;
    lst -> capacity = realloc_coeff * lst -> capacity;

    return 0;
}

int list_erase(list_t* lst, lst_index_t i)
{
    assert(lst);
    if (PREV(i) == -1)
    {
        fprintf(stderr, "LIST ERROR: Cannot erase free element\n");
        return -1;
    }

    PREV(NEXT(i)) = PREV(i);
    NEXT(PREV(i)) = NEXT(i);

    PREV(i) = -1;
    NEXT(i) = lst -> free;
    lst -> free = i;

    if (PREV(0) == 0)
    {
        PREV(0) = lst -> free;
    }
    return 0;
}

#undef NEXT
#undef PREV
