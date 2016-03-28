#include <stdlib.h>
#include <stdlib.h>

#include "graph.h"

/* malloc() / free() testing */

#ifdef ALLOC_TESTING
#include "alloc-testing.h"
#endif

static int graph_add_new_vertex(Graph * g, VertexValue value) {
    if(g == NULL)
        return 0;
    Vertex * newV = (Vertex *)malloc(sizeof(Vertex));
    if(newV == NULL)
        return 0;
    newV->edges = NULL;
    newV->value = value;
    list_append(&(g->vertices), newV);
    ++g->vertexNum;
    return 1;
}

static int undirected_add_new_edge(Graph * g, Vertex * sourceVertex, Vertex * targetVertex, EdgeValue value) {
    if(g == NULL)
        return 0;
    Edge * newE = (Edge *)malloc(sizeof(Edge));
    if(newE == NULL)
        return 0;
    newE->vertices[0] = sourceVertex;
    newE->vertices[1] = targetVertex;
    newE->value = value;
    list_append(&(sourceVertex->edges), newE);
    list_append(&(targetVertex->edges), newE);
    list_append(&(g->edges), newE);

    ++g->edgeNum;
}

static void graph_free(Graph * g) {
    list_free(g->vertices);
    list_free(g->edges);
    free(g);
}

Graph * graph_new(GraphType type) {
    Graph * newGraph;

    newGraph = (Graph *) malloc(sizeof(Graph));

    if (newGraph == NULL) {
        return NULL;
    }
    newGraph->graphType = type;
    /*初始化二叉树*/
    graph_initial(newGraph);

    return newGraph;
}

void graph_initial(Graph * g) {
    if (g == NULL)
        return;
    g->vertexNum = 0;
    g->edgeNum = 0;
    g->vertices = NULL;
    g->add_new_vertex = &graph_add_new_vertex;
    g->graph_free = &graph_free;
    if(g->graphType == GRAPH_UNDIRECTED) {
        g->add_new_edge = &undirected_add_new_edge;
    } else {

    }
}