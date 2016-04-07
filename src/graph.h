#ifndef ALGORITHM_GRAPH_H
#define ALGORITHM_GRAPH_H

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GRAPH_UNDIRECTED = 0,
    GRAPH_DIRECTED = 1
} GraphType;

typedef struct _Graph Graph;

typedef struct _Vertex Vertex;

typedef struct _Edge Edge;

typedef struct _DijkstraPath DijkstraPath;

typedef struct _MinSpanningTree MinSpanningTree;

typedef void * EdgeValue;

typedef void * VertexValue;

struct _Edge {
    Vertex * vertices[2];
    EdgeValue value;
    float weight;
};

struct _Vertex {
    int id;
    ListEntry * edges;
    VertexValue value;
};

typedef void (*traverse_callback)(Vertex * v, int subGraphNo);

struct _DijkstraPath {
    float * pathCosts;
    int * vertexIds;
};

struct _MinSpanningTree {
    float spanningTreeCost;
    ListEntry * edges;
};

struct _Graph {
    GraphType  graphType;
    /* count of vertex and edge */
    unsigned int vertexNum;
    unsigned int edgeNum;
    /* vertex list */
    ListEntry * vertices;
    ListEntry * edges;

    Vertex * (*add_new_vertex)(Graph *_this, VertexValue value);
    Edge * (*add_new_edge)(Graph *_this, Vertex * sourceVertex, Vertex * targetVertex, EdgeValue value);
    void (*traverse_bfs)(Graph *_this, Vertex * firstVertex, traverse_callback callBack);
    void (*traverse_dfs)(Graph *_this, Vertex * firstVertex, traverse_callback callBack);
    MinSpanningTree * (*prim_spanning_tree)(Graph *_this);
    MinSpanningTree * (*kruskal_spanning_tree)(Graph *_this);
    void (*dijkstra)(Graph *_this, Vertex * start, DijkstraPath * path);
    float ** (*floyd)(Graph *_this);
    void (*graph_free)(Graph *_this);
};

Graph * graph_new(GraphType type);

void graph_initial(Graph * g);

int critical_path_method(Graph *_this, Vertex * start, float * earlyTime, float * lateTime);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ALGORITHM_GRAPH_H */

