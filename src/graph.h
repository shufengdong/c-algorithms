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

typedef void * EdgeValue;

typedef void * VertexValue;

struct _Edge {
    Vertex * vertices[2];
    EdgeValue value;
};

struct _Vertex {
    ListEntry * edges;
    VertexValue value;
};

typedef void (*traverse_callback)(Vertex * v);

struct _Graph {
    GraphType  graphType;
    /* count of vertex and edge */
    unsigned int vertexNum;
    unsigned int edgeNum;
    /* vertex list */
    ListEntry * vertices;
    ListEntry * edges;

    int (*add_new_vertex)(Graph *_this, VertexValue value);
    int (*add_new_edge)(Graph *_this, Vertex * sourceVertex, Vertex * targetVertex, EdgeValue value);
    void (*graph_bfs)(Graph *_this, Vertex * firstVertex, traverse_callback callBack);
    void (*graph_dfs)(Graph *_this, Vertex * firstVertex, traverse_callback callBack);
    void (*graph_free)(Graph *_this);
};


Graph * graph_new(GraphType type);

void graph_initial(Graph * g);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ALGORITHM_GRAPH_H */

