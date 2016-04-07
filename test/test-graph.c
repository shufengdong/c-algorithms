#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "alloc-testing.h"
#include "framework.h"

#include "graph.h"
#include "float.h"

static void graph_traverse_callback(Vertex * v, int subGraphNo) {
    printf("%d---->%d\n", subGraphNo, *(int *)(v->value));
}

static int values[] = {1, 2, 3, 4, 5, 6, 7, 8};
static int edgeValues[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

static void fill_graph(Graph * g) {
    int i, n = 8;
    /*插入一些数据*/
    for(i = 0; i < n; i++)
        g->add_new_vertex(g, &values[i]);
    g->add_new_edge(g, list_nth_data(g->vertices, 0), list_nth_data(g->vertices, 1), &edgeValues[0]);
    g->add_new_edge(g, list_nth_data(g->vertices, 1), list_nth_data(g->vertices, 3), &edgeValues[1]);
    g->add_new_edge(g, list_nth_data(g->vertices, 3), list_nth_data(g->vertices, 7), &edgeValues[2]);
    g->add_new_edge(g, list_nth_data(g->vertices, 4), list_nth_data(g->vertices, 7), &edgeValues[3]);
    g->add_new_edge(g, list_nth_data(g->vertices, 4), list_nth_data(g->vertices, 1), &edgeValues[4]);
    g->add_new_edge(g, list_nth_data(g->vertices, 0), list_nth_data(g->vertices, 2), &edgeValues[5]);
    g->add_new_edge(g, list_nth_data(g->vertices, 2), list_nth_data(g->vertices, 5), &edgeValues[6]);
    g->add_new_edge(g, list_nth_data(g->vertices, 5), list_nth_data(g->vertices, 6), &edgeValues[7]);
    g->add_new_edge(g, list_nth_data(g->vertices, 2), list_nth_data(g->vertices, 6), &edgeValues[8]);
}

static void test_graph_critical_path_method() {
    Graph * g = graph_new(GRAPH_DIRECTED);
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int edgeValues[] = {6, 4, 5, 1, 1, 2, 9, 7, 4, 2, 4};
    int i, n = 9;
    /*插入一些数据*/
    for(i = 0; i < n; i++) {
        Vertex * v = g->add_new_vertex(g, &values[i]);
        assert(v->id == (i + 1));
        assert(*((int *)v->value) == values[i]);
    }
    Edge * e;
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 0), (Vertex *)list_nth_data(g->vertices, 1), &edgeValues[0]);
    e->weight = edgeValues[0];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 0), (Vertex *)list_nth_data(g->vertices, 2), &edgeValues[1]);
    e->weight = edgeValues[1];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 0), (Vertex *)list_nth_data(g->vertices, 3), &edgeValues[2]);
    e->weight = edgeValues[2];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 1), (Vertex *)list_nth_data(g->vertices, 4), &edgeValues[3]);
    e->weight = edgeValues[3];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 2), (Vertex *)list_nth_data(g->vertices, 4), &edgeValues[4]);
    e->weight = edgeValues[4];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 3), (Vertex *)list_nth_data(g->vertices, 5), &edgeValues[5]);
    e->weight = edgeValues[5];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 4), (Vertex *)list_nth_data(g->vertices, 6), &edgeValues[6]);
    e->weight = edgeValues[6];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 4), (Vertex *)list_nth_data(g->vertices, 7), &edgeValues[7]);
    e->weight = edgeValues[7];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 5), (Vertex *)list_nth_data(g->vertices, 7), &edgeValues[8]);
    e->weight = edgeValues[8];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 6), (Vertex *)list_nth_data(g->vertices, 8), &edgeValues[9]);
    e->weight = edgeValues[9];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 7), (Vertex *)list_nth_data(g->vertices, 8), &edgeValues[10]);
    e->weight = edgeValues[10];

    float earlyTime[n];
    float lateTime[n];
    int r = critical_path_method(g, list_nth_data(g->vertices, 0), earlyTime, lateTime);
    assert(r == 1);
    assert(earlyTime[0] == 0);
    assert(earlyTime[1] == 6);
    assert(earlyTime[2] == 4);
    assert(earlyTime[3] == 5);
    assert(earlyTime[4] == 7);
    assert(earlyTime[5] == 7);
    assert(earlyTime[6] == 16);
    assert(earlyTime[7] == 14);
    assert(earlyTime[8] == 18);
    assert(lateTime[0] == 0);
    assert(lateTime[1] == 6);
    assert(lateTime[2] == 6);
    assert(lateTime[3] == 8);
    assert(lateTime[4] == 7);
    assert(lateTime[5] == 10);
    assert(lateTime[6] == 16);
    assert(lateTime[7] == 14);
    assert(lateTime[8] == 18);

    g->graph_free(g);
}

static void test_graph_dijkstra_avl() {
    Graph * g = graph_new(GRAPH_DIRECTED);
    int values[] = {1, 2, 3, 4, 5, 6};
    int edgeValues[] = {10,30,100,5,50,10,20,60};
    int i, n = 6;
    /*插入一些数据*/
    for(i = 0; i < n; i++) {
        Vertex * v = g->add_new_vertex(g, &values[i]);
        assert(v->id == (i + 1));
        assert(*((int *)v->value) == values[i]);
    }
    Edge * e;
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 0), (Vertex *)list_nth_data(g->vertices, 2), &edgeValues[0]);
    e->weight = edgeValues[0];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 0), (Vertex *)list_nth_data(g->vertices, 4), &edgeValues[1]);
    e->weight = edgeValues[1];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 0), (Vertex *)list_nth_data(g->vertices, 5), &edgeValues[2]);
    e->weight = edgeValues[2];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 1), (Vertex *)list_nth_data(g->vertices, 2), &edgeValues[3]);
    e->weight = edgeValues[3];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 2), (Vertex *)list_nth_data(g->vertices, 3), &edgeValues[4]);
    e->weight = edgeValues[4];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 3), (Vertex *)list_nth_data(g->vertices, 5), &edgeValues[5]);
    e->weight = edgeValues[5];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 4), (Vertex *)list_nth_data(g->vertices, 3), &edgeValues[6]);
    e->weight = edgeValues[6];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 4), (Vertex *)list_nth_data(g->vertices, 5), &edgeValues[7]);
    e->weight = edgeValues[7];

    float nodeCosts[g->vertexNum];
    int nodeIds[g->vertexNum];
    DijkstraPath * path = (DijkstraPath *)malloc(sizeof(DijkstraPath));
    path->pathCosts = nodeCosts;
    path->vertexIds = nodeIds;
    g->dijkstra(g, list_nth_data(g->vertices, 0), path);
    assert(*(path->vertexIds) == 1);
    assert(*(path->vertexIds + 1) == 3);
    assert(*(path->vertexIds + 2) == 5);
    assert(*(path->vertexIds + 3) == 4);
    assert(*(path->vertexIds + 4) == 6);
    assert(*(path->vertexIds + 5) == -1);
    assert(*(path->pathCosts) == 0);
    assert(*(path->pathCosts + 1) == FLT_MAX);
    assert(*(path->pathCosts + 2) == 10);
    assert(*(path->pathCosts + 3) == 50);
    assert(*(path->pathCosts + 4) == 30);
    assert(*(path->pathCosts + 5) == 60);
    free(path);

    /*测试floyd方法*/
    float ** d = g->floyd(g);
    assert(*(*(d + 0) + 0) == 0);
    assert(*(*(d + 0) + 1) == FLT_MAX);
    assert(*(*(d + 0) + 2) == 10);
    assert(*(*(d + 0) + 3) == 50);
    assert(*(*(d + 0) + 4) == 30);
    assert(*(*(d + 0) + 5) == 60);
    int j;
    for(i = 0; i < n; i++)
        for(j = 0; j < n; j++)
            if(*(*(d+i) + j) == FLT_MAX)
                printf("%d %d FLT_MAX\n", i, j);
            else
                printf("%d %d %f\n", i, j, *(*(d+i) + j));
    for(i = 0; i < n; i++)
        free(*(d + i));
    free(d);
    g->graph_free(g);
}

static void test_graph_spanning_tree() {
    Graph * g = graph_new(GRAPH_UNDIRECTED);
    int values[] = {1, 2, 3, 4, 5, 6};
    int edgeValues[] = {6,1,5,5,3,5,6,4,2,6};
    int i, n = 6;
    /*插入一些数据*/
    for(i = 0; i < n; i++) {
        Vertex * v= g->add_new_vertex(g, &values[i]);
        assert(v->id == (i + 1));
        assert(*((int *)v->value) == values[i]);
    }
    Edge * e;
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 0), (Vertex *)list_nth_data(g->vertices, 1), &edgeValues[0]);
    e->weight = edgeValues[0];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 0), (Vertex *)list_nth_data(g->vertices, 2), &edgeValues[1]);
    e->weight = edgeValues[1];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 0), (Vertex *)list_nth_data(g->vertices, 3), &edgeValues[2]);
    e->weight = edgeValues[2];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 1), (Vertex *)list_nth_data(g->vertices, 2), &edgeValues[3]);
    e->weight = edgeValues[3];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 1), (Vertex *)list_nth_data(g->vertices, 4), &edgeValues[4]);
    e->weight = edgeValues[4];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 2), (Vertex *)list_nth_data(g->vertices, 3), &edgeValues[5]);
    e->weight = edgeValues[5];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 2), (Vertex *)list_nth_data(g->vertices, 4), &edgeValues[6]);
    e->weight = edgeValues[6];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 2), (Vertex *)list_nth_data(g->vertices, 5), &edgeValues[7]);
    e->weight = edgeValues[7];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 3), (Vertex *)list_nth_data(g->vertices, 5), &edgeValues[8]);
    e->weight = edgeValues[8];
    e = g->add_new_edge(g, (Vertex *)list_nth_data(g->vertices, 4), (Vertex *)list_nth_data(g->vertices, 5), &edgeValues[9]);
    e->weight = edgeValues[9];

    MinSpanningTree * tree = g->kruskal_spanning_tree(g);
    assert(tree != NULL);
    assert(tree->spanningTreeCost == 15.0);
    list_free(tree->edges);
    free(tree);

    tree = g->prim_spanning_tree(g);
    assert(tree != NULL);
    assert(tree->spanningTreeCost == 15.0);
    list_free(tree->edges);
    free(tree);

    g->graph_free(g);
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
    test_graph_spanning_tree,
    test_graph_dijkstra_avl,
    test_graph_critical_path_method,
    NULL
};

int main(int argc, char *argv[]) {
    run_tests(tests);
    return 0;
}