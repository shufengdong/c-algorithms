#include <stdlib.h>
#include <stdlib.h>

#include "graph.h"
#include "queue.h"

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
    newV->id = g->vertexNum;
    return 1;
}

static int graph_add_new_edge(Graph * g, Vertex * sourceVertex, Vertex * targetVertex, EdgeValue value) {
    if(g == NULL)
        return 0;
    Edge * newE = (Edge *)malloc(sizeof(Edge));
    if(newE == NULL)
        return 0;
    newE->vertices[0] = sourceVertex;
    newE->vertices[1] = targetVertex;
    newE->value = value;
    list_append(&(sourceVertex->edges), newE);
    if(g->graphType == GRAPH_UNDIRECTED)
        list_append(&(targetVertex->edges), newE);
    list_append(&(g->edges), newE);
    ++g->edgeNum;
}

static void graph_traverse_dfs_impl(Graph * g, Vertex * start, char * visited, traverse_callback callBack) {
    callBack(start);
    visited[start->id - 1] = '1';
    /*处理与当前顶点连接的顶点*/
    ListIterator iter;
    list_iterate(&start->edges, &iter);
    while (list_iter_has_more(&iter)) {
        Vertex * neighbor;
        Edge * edge = (Edge *) list_iter_next(&iter);
        /*根据边寻找下一个顶点*/
        if(g->graphType == GRAPH_UNDIRECTED) {
            if(edge->vertices[0] == start)
                neighbor = edge->vertices[1];
            else
                neighbor = edge->vertices[0];
        } else
            neighbor = edge->vertices[1];
        /* do nothing if the node is already visited */
        if(visited[neighbor->id - 1] != '0')
            continue;
        graph_traverse_dfs_impl(g, neighbor, visited, callBack);
    }
}

static void graph_traverse_dfs(Graph * g, Vertex * start, traverse_callback callBack) {
    char visited[g->vertexNum];
    int i, isFinished = 0;
    for(i = 0; i < g->vertexNum; i++)
        visited[i] = '0';
    Vertex * firstVertex = start;
    do {
        graph_traverse_dfs_impl(g, firstVertex, visited, callBack);
        isFinished = 1;
        //判断是否已经搜索完毕
        for(i = 0; i < g->vertexNum; i++) {
            if(visited[i] == '0') {
                isFinished = 0;
                break;
            }
        }
        if(isFinished)
            break;
        else
            firstVertex = list_nth_data(g->vertices, i);
    } while(1);

}

static void graph_traverse_bfs(Graph * g, Vertex * start, traverse_callback callBack) {
    if(callBack == NULL)
        return;
    Vertex * neighbor;
    Vertex * currentNode;
    Edge * edge;
    int i, isFinished = 0;

    char visited[g->vertexNum];
    for(i = 0; i < g->vertexNum; i++)
        visited[i] = '0';

    Queue * queue = queue_new();
    queue_push_tail(queue, start);

    do {
        while(!queue_is_empty(queue)) {
            currentNode = (Vertex *)queue_pop_head(queue);
            visited[currentNode->id - 1] = '2';
            /*调用回调函数*/
            callBack(currentNode);

            /*处理与当前顶点连接的顶点*/
            ListIterator iter;
            list_iterate(&currentNode->edges, &iter);
            while (list_iter_has_more(&iter)) {
                edge = (Edge *) list_iter_next(&iter);
                /*根据边寻找下一个顶点*/
                if(g->graphType == GRAPH_UNDIRECTED) {
                    if(edge->vertices[0] == currentNode)
                        neighbor = edge->vertices[1];
                    else
                        neighbor = edge->vertices[0];
                } else
                    neighbor = edge->vertices[1];
                /* do nothing if the node is already visited */
                if(visited[neighbor->id - 1] != '0')
                    continue;
                visited[neighbor->id - 1] = '1';
                queue_push_tail(queue, neighbor);
            }
        }
        //判断是否已经搜索完毕
        isFinished = 1;
        for(i = 0; i < g->vertexNum; i++) {
            if(visited[i] == '0') {
                isFinished = 0;
                break;
            }
        }
        if(isFinished)
            break;
        else /*如果还有没有结束,从第一个没处理过的节点开始继续遍历*/
            queue_push_tail(queue, list_nth_data(g->vertices, i));
    } while(1);

    queue_free(queue);
    return NULL;
}


static void graph_free(Graph * g) {
    /*清理顶点和边的内存*/
    ListIterator iter;
    list_iterate(&g->vertices, &iter);
    while (list_iter_has_more(&iter))
        free(list_iter_next(&iter));
    list_iterate(&g->edges, &iter);
    while (list_iter_has_more(&iter))
        free(list_iter_next(&iter));
    /*清理列表的内存*/
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
    g->edges = NULL;
    g->graph_free = &graph_free;
    g->add_new_vertex = &graph_add_new_vertex;
    g->add_new_edge = &graph_add_new_edge;
    g->traverse_bfs = &graph_traverse_bfs;
    g->traverse_dfs = &graph_traverse_dfs;
}