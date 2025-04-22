#include "my_list.h"

#define NEXT(i) lst -> next[i]
#define PREV(i) lst -> prev[i]

FILE* prepare_to_dump()
{
    FILE* fp = fopen("list_dump.html", "w");
    if (fp == NULL)
    {
        fprintf(stderr, "ERROR: Cannot open html file fot dump\n");
        return NULL;
    }
    fprintf(fp, "<pre>\n");

    return fp;
}

int list_dump(list_t* lst, FILE* html_stream)
{
    static lst_index_t dump_counter = 1;
    char dot_file_name[32] = "";
    char png_file_name[32] = "";
    sprintf(dot_file_name, "data/dump%d.dot", dump_counter);
    sprintf(png_file_name, "data/dump%d.png", dump_counter);

    FILE* fp = fopen(dot_file_name, "w");
    if (fp == NULL)
    {
        fprintf(stderr, "ERROR: Unable to open file for dump\n");
        return -1;
    }

    make_dot_file(lst, fp);
    fclose(fp);

    char command[BUFSIZ] = "";
    sprintf(command, "dot %s -Tpng -o %s\n", dot_file_name, png_file_name);

    system(command);

    dump_to_html(lst, png_file_name, html_stream);

    dump_counter++;
    return 0;
};

int make_dot_file(list_t* lst, FILE* fp)
{
    assert(fp);
    assert(lst);

    fprintf(fp, "digraph\n{\n"
                "    graph[splines = ortho]\n"
                "    rankdir=LR\n\n"
                "    ");

    for (lst_index_t k = 0; k < lst -> capacity - 1; k++)
    {
        fprintf(fp, "\"%d\" -> ", k);
    }
    fprintf(fp, "\"%d\";\n\n", lst -> capacity - 1);

    int i = lst -> free;
    fprintf(fp, "    node%d[shape=record,style=\"rounded,filled\",fillcolor=\"#163bf3\","
                    "label=\"index: %d | data: %s | count: %ld | next: %d | prev: %d\"];\n",
                    i, i, lst -> data[i].buffer,  lst -> data[i].counter, NEXT(i), PREV(i));
    i = NEXT(i);
    while (i != 0)
    {
        fprintf(fp, "    node%d[shape=record,style=\"rounded,filled\",fillcolor=\"#39CCCC\","
                    "label=\"index: %d | data: %s | count: %ld | next: %d | prev: %d\"];\n",
                    i, i, lst -> data[i].buffer, lst -> data[i].counter, NEXT(i), PREV(i));
        i = NEXT(i);
    }


    fprintf(fp, "    node%d[shape=record,style=\"rounded,filled\",fillcolor=\"#BE08F0\","
                "label=\"index: %d | data: %s | count: %ld | next: %d | prev: %d\"];\n",
                0, 0, lst -> data[0].buffer,  lst -> data[i].counter, NEXT(0), PREV(0));
    i = NEXT(0);
    while (i != 0)
    {
        fprintf(fp, "    node%d[shape=record,style=\"rounded,filled\",fillcolor=\"#2ECC40\","
                    "label=\"index: %d | data: %s | count: %ld | next: %d | prev: %d\"];\n",
                    i, i, lst -> data[i].buffer, lst -> data[i].counter, NEXT(i), PREV(i));
        i = NEXT(i);
    }

    fprintf(fp, "\n");
    for (lst_index_t k = 0; k < lst -> capacity - 1; k++)
    {
        fprintf(fp, "    node%d -> node%d[color=\"none\"]\n"
                    "    {rank = same; \"%d\"; node%d}\n", k, k + 1, k, k);
    }
    fprintf(fp, "    {rank = same; \"%d\"; node%d}\n\n", lst -> capacity - 1, lst -> capacity - 1);

    i = 0;

    while (i != PREV(0) && NEXT(0) != 0) //last case differ
    {
        fprintf(fp, "    node%d -> node%d[color=\"#0855F0\",constraint=false]\n", i, NEXT(i));
        fprintf(fp, "    node%d -> node%d[color=\"#F00822\",constraint=false]\n", NEXT(i), i);
        i = NEXT(i);
    }
    fprintf(fp, "    node%d -> node0[color=\"#0855F0\",constraint=false]\n\n", i);
    fprintf(fp, "    node%d -> node%d[color=\"#F00822\",constraint=false]\n", NEXT(i), i);

    i = lst -> free;
    while (i != 0)
    {
        fprintf(fp, "    node%d -> node%d[color=\"#0855F0\",constraint=false]\n", i, NEXT(i));
        i = NEXT(i);
    }

    fprintf(fp, "}");

    printf("dot file completed\n");
    return 0;
}

int dump_to_html(list_t* lst, char* png_file_name, FILE* html_stream)
{
    assert(lst);
    assert(png_file_name);

    fprintf(html_stream, "---LIST DUMP---\nMEMORY PRINT\n");
    fprintf(html_stream, "curr data next prev\n");
    for (lst_index_t i = 0; i < lst -> capacity; i++)
    {
        fprintf(html_stream, "%4d %s %4d %4d\n", i, lst -> data[i].buffer, lst -> next[i], lst -> prev[i]);
    }
    fprintf(html_stream, "\ncapacity: %d\n", lst -> capacity);
    fprintf(html_stream, "free: %d\n", lst -> free);
    fprintf(html_stream, "<img src=%s>\n", png_file_name);

    return 0;
}
