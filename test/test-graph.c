#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "alloc-testing.h"
#include "framework.h"

#include "graph.h"

void test_graph_new() {
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8};
    int i, n = 8;

    Graph * g = graph_new(GRAPH_UNDIRECTED);

    assert(g != NULL);
    assert(g->vertices == NULL);
    assert(g->vertexNum == 0);
    assert(g->edgeNum == 0);
    /*插入一些数据*/
    for(i = 0; i < n; i++)
        g->add_new_vertex(g, &values[i]);
    printf("%d\n", g->vertexNum);
    assert(g->vertexNum == 8);
    g->graph_free(g);
}

static UnitTestFunction tests[] = {
    test_graph_new,
    NULL
};

int main(int argc, char *argv[]) {
    run_tests(tests);
    return 0;
}