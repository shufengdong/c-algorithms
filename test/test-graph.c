#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "alloc-testing.h"
#include "framework.h"

#include "graph.h"

static void graph_traverse_callback(Vertex * v) {
    printf("%d\n", *(int *)(v->value));
}

static int values[] = {1, 2, 3, 4, 5, 6, 7, 8};
static int edgeValues[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

static void fill_graph(Graph * g) {
    int i, n = 8;
    /*插入一些数据*/
    for(i = 0; i < n; i++)
        g->add_new_vertex(g, &values[i]);
    g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 0),
        (Vertex *)list_nth_data(g->vertices, 1), &edgeValues[0]);
    g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 1),
        (Vertex *)list_nth_data(g->vertices, 3), &edgeValues[1]);
    g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 3),
        (Vertex *)list_nth_data(g->vertices, 7), &edgeValues[2]);
    g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 4),
        (Vertex *)list_nth_data(g->vertices, 7), &edgeValues[3]);
    g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 4),
        (Vertex *)list_nth_data(g->vertices, 1), &edgeValues[4]);
    g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 0),
        (Vertex *)list_nth_data(g->vertices, 2), &edgeValues[5]);
    g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 2),
        (Vertex *)list_nth_data(g->vertices, 5), &edgeValues[6]);
    g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 5),
        (Vertex *)list_nth_data(g->vertices, 6), &edgeValues[7]);
    g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 2),
        (Vertex *)list_nth_data(g->vertices, 6), &edgeValues[8]);
}


static void test_graph_traverse_directed() {
    Graph * g = graph_new(GRAPH_DIRECTED);
    fill_graph(g);
    g->traverse_bfs(g, (Vertex *)list_nth_data(g->vertices, 0), &graph_traverse_callback);
    g->traverse_dfs(g, (Vertex *)list_nth_data(g->vertices, 0), &graph_traverse_callback);
    g->graph_free(g);
}

static void test_graph_traverse() {
    Graph * g = graph_new(GRAPH_UNDIRECTED);
    fill_graph(g);
    g->traverse_bfs(g, (Vertex *)list_nth_data(g->vertices, 0), &graph_traverse_callback);
    g->traverse_dfs(g, (Vertex *)list_nth_data(g->vertices, 0), &graph_traverse_callback);
    g->graph_free(g);
}

static void test_graph_new() {
    Graph * g = graph_new(GRAPH_UNDIRECTED);
    assert(g != NULL);
    assert(g->vertices == NULL);
    assert(g->vertexNum == 0);
    assert(g->edgeNum == 0);

    fill_graph(g);

    assert(g->vertexNum == 8);
    assert(g->edgeNum == 9);
    g->graph_free(g);
}

static UnitTestFunction tests[] = {
    test_graph_new,
    test_graph_traverse,
    test_graph_traverse_directed,
    NULL
};

int main(int argc, char *argv[]) {
    run_tests(tests);
    return 0;
}