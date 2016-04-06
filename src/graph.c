#include <stdlib.h>
#include <stdlib.h>

#include <float.h>

#include "graph.h"
#include "queue.h"
#include "compare-pointer.h"
#include "avl-tree.h"
#include "float.h"
/* malloc() / free() testing */

#ifdef ALLOC_TESTING
#include "alloc-testing.h"
#endif

#define min(a,b) (((a)>(b)) ? (b):(a))
#define max(a,b) (((a)>(b)) ? (a):(b))

static Vertex * graph_add_new_vertex(Graph * g, VertexValue value) {
    if(g == NULL)
        return NULL;
    Vertex * newV = (Vertex *)malloc(sizeof(Vertex));
    if(newV == NULL)
        return NULL;
    newV->edges = NULL;
    newV->value = value;
    list_append(&(g->vertices), newV);
    ++g->vertexNum;
    newV->id = g->vertexNum;
    return newV;
}

static Edge * graph_add_new_edge(Graph * g, Vertex * sourceVertex, Vertex * targetVertex, EdgeValue value) {
    if(g == NULL)
        return NULL;
    Edge * newE = (Edge *)malloc(sizeof(Edge));
    if(newE == NULL)
        return NULL;
    newE->vertices[0] = sourceVertex;
    newE->vertices[1] = targetVertex;
    newE->value = value;
    newE->weight = 1.0;
    list_append(&(sourceVertex->edges), newE);
    if(g->graphType == GRAPH_UNDIRECTED)
        list_append(&(targetVertex->edges), newE);
    list_append(&(g->edges), newE);
    ++g->edgeNum;
    return newE;
}

static void graph_traverse_dfs_impl(Graph * g, Vertex * start, char * visited,
                                    traverse_callback callBack, int subGraphNo) {
    callBack(start, subGraphNo);
    visited[start->id - 1] = '1';
    /*处理与当前顶点连接的顶点*/
    ListIterator iter;
    list_iterate(&start->edges, &iter);
    while (list_iter_has_more(&iter)) {
        Vertex * neighbor;
        Edge * edge = list_iter_next(&iter);
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
        graph_traverse_dfs_impl(g, neighbor, visited, callBack, subGraphNo);
    }
}

static void graph_traverse_dfs(Graph * g, Vertex * start, traverse_callback callBack) {
    char visited[g->vertexNum];
    int i, isFinished = 0, subGraphNo = 0;
    for(i = 0; i < g->vertexNum; i++)
        visited[i] = '0';
    Vertex * firstVertex = start;
    do {
        graph_traverse_dfs_impl(g, firstVertex, visited, callBack, subGraphNo);
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
        ++subGraphNo;
    } while(1);
}

static void graph_traverse_bfs(Graph * g, Vertex * start, traverse_callback callBack) {
    if(callBack == NULL)
        return;
    Vertex * neighbor;
    Vertex * currentNode;
    Edge * edge;
    int i, isFinished = 0, subGraphNo = 0;

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
            callBack(currentNode, subGraphNo);

            /*处理与当前顶点连接的顶点*/
            ListIterator iter;
            list_iterate(&currentNode->edges, &iter);
            while (list_iter_has_more(&iter)) {
                edge = list_iter_next(&iter);
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
        ++subGraphNo;
    } while(1);

    queue_free(queue);
}

static compareEdgeWeight(void * e1, void * e2) {
    if(((Edge *)e1)->weight < ((Edge *)e2)->weight)
        return -1;
    else if(((Edge *)e1)->weight == ((Edge *)e2)->weight)
        return 0;
    else
        return 1;
}

static MinSpanningTree * graph_kruskal_spanning_tree(Graph * g) {
    MinSpanningTree * tree = (MinSpanningTree *)malloc(sizeof(MinSpanningTree));
    if(tree == NULL)
        return NULL;
    tree->spanningTreeCost = 0.0;
    tree->edges = NULL;

    ListEntry * nodes = NULL; /*存储树中的节点*/
    ListEntry * edges = NULL; /*存储排序后的边*/
    ListIterator iter;
    list_iterate(&g->edges, &iter);
    while (list_iter_has_more(&iter))
        list_append(&edges, list_iter_next(&iter));
    /*排序*/
    list_sort(&edges, &compareEdgeWeight);
    int isEgeNeedAdd = 0;
    list_iterate(&edges, &iter);
    int count = 0;
    while (list_iter_has_more(&iter)) {
        Edge * e = list_iter_next(&iter);
        isEgeNeedAdd = 0;
        //todo: 效率不高，改用hashmap重写
        if(list_find_data(nodes, &pointer_equal, e->vertices[0]) == NULL) {
            list_append(&nodes, e->vertices[0]);
            isEgeNeedAdd = 1;
        }
        if(list_find_data(nodes, &pointer_equal, e->vertices[1]) == NULL) {
            list_append(&nodes, e->vertices[1]);
            isEgeNeedAdd = 1;
        }
        if(isEgeNeedAdd) {
            list_append(&tree->edges, list_iter_next(&iter));
            tree->spanningTreeCost += e->weight;
        }
        if(count == g->vertexNum -1)
            break;
        count++;
    }
    list_free(edges);
    list_free(nodes);
    return tree;
}

static MinSpanningTree * graph_prim_spanning_tree(Graph * g) {
    MinSpanningTree * tree = (MinSpanningTree *)malloc(sizeof(MinSpanningTree));
    if(tree == NULL)
        return NULL;
    tree->spanningTreeCost = 0.0;
    tree->edges = NULL;

    ListEntry * nodes = NULL; /*存储树中的节点*/

    Vertex * nextVertex = list_nth_data(g->vertices, 0);
    list_append(&nodes, nextVertex);
    float minWeight;
    int i;
    Edge * nextEdge = NULL;
    for(i = 0; i < g->vertexNum - 1; i++) {
        ListIterator iter;
        list_iterate(&nodes, &iter);
        minWeight = FLT_MAX;
        while (list_iter_has_more(&iter)) {
            Vertex * node = list_iter_next(&iter);
            ListIterator iter2;
            list_iterate(&node->edges, &iter2);
            while (list_iter_has_more(&iter2)) {
                Edge * edge = list_iter_next(&iter2);
                if(edge->vertices[0] == node) {
                //todo: 效率不高，改用hashmap重写
                if(list_find_data(nodes, &pointer_equal, edge->vertices[1]) == NULL)
                        if(edge->weight < minWeight) {
                            minWeight = edge->weight;
                            nextEdge = edge;
                            nextVertex = edge->vertices[1];
                        }
                } else {
                    if(list_find_data(nodes, &pointer_equal, edge->vertices[0]) == NULL)
                        if(edge->weight < minWeight) {
                            minWeight = edge->weight;
                            nextEdge = edge;
                            nextVertex = edge->vertices[0];
                        }
                }
            }
        }
        tree->spanningTreeCost += minWeight;
        list_append(&nodes, nextVertex);
        list_append(&tree->edges, nextEdge);
    }
    list_free(nodes);
    return tree;
}

typedef struct _DijkstraPathNode {
    Vertex * vertex;
    float cost;
} DijkstraPathNode;

//todo: 统一放在compare.c中
static float_compare(void * v1, void * v2) {
    if (((DijkstraPathNode *) v1)->cost < ((DijkstraPathNode *) v2)->cost) {
        return -1;
    } else if (((DijkstraPathNode *) v1)->cost > ((DijkstraPathNode *) v2)->cost) {
        return 1;
    } else {
        return 0;
   }
}


static float ** graph_floyd(Graph * g) {
    int i, j, k;
    float ** d = (float **)malloc(g->vertexNum * sizeof(float*));
    if(d == NULL)
        return NULL;
    for(i = 0; i < g->vertexNum; i++) {
        *(d+i) = (float *)malloc(g->vertexNum * sizeof(float));
        if(*(d+i) == NULL) {
            for(j = 0; j < i - 1; j++)
                free(*(d + j));
            free(d);
            return NULL;
        }
    }
    //赋初值
    for(i = 0; i < g->vertexNum; i++)
        for(j = 0; j < g->vertexNum; j++)
            *(*(d + i) + j) = FLT_MAX;
    for(i = 0; i < g->vertexNum; i++)
        *(*(d + i) + i) = 0.0;
    ListIterator iter;
    list_iterate(&g->edges, &iter);
    while (list_iter_has_more(&iter)) {
        Edge * e = list_iter_next(&iter);
        *(*(d + e->vertices[0]->id - 1) + e->vertices[1]->id -1) = e->weight;
        if(g->graphType == GRAPH_UNDIRECTED)
            *(*(d + e->vertices[1]->id - 1) + e->vertices[0]->id -1) = e->weight;
    }
    //开始floyd算法
    for(k = 0; k < g->vertexNum; k++)
        for(i = 0; i < g->vertexNum; i++)
            for(j = 0; j < g->vertexNum; j++)
                *(*(d + i) + j) = min(*(*(d + i) + j), *(*(d + i) + k) + *(*(d + k) + j));
    return d;
}

static DijkstraPath * graph_dijkstra_avl(Graph * g, Vertex * start) {
    DijkstraPath * path = (DijkstraPath *)malloc(sizeof(DijkstraPath));
    if(path == NULL)
        return NULL;
    path->pathCosts = (float *)malloc(g->vertexNum * sizeof(float));
    if(path->pathCosts == NULL) {
        free(path);
        return NULL;
    }
    path->vertexIds = (int *)malloc(g->vertexNum * sizeof(int));
    if(path->vertexIds == NULL) {
        free(path->pathCosts);
        free(path);
        return NULL;
    }
    DijkstraPathNode * nodes = (DijkstraPathNode *)malloc(g->vertexNum * sizeof(DijkstraPathNode));
    if(nodes == NULL) {
        free(path->pathCosts);
        free(path->vertexIds);
        free(path);
        return NULL;
    }


    int i, j, isHandled;
    float oldCost, newCost;
    for(i = 0; i < g->vertexNum; i++) {
        *(path->pathCosts + i) = FLT_MAX;
        *(path->vertexIds + i) = -1;
    }
    /*将自身的路径距离置为0*/
    nodes->vertex = start;
    nodes->cost = 0.0;

    Vertex * minVertex = NULL, * adjacentVertex;
    Edge * edge = NULL;
    AVLTreeNode * minNode;

    AVLTree * tree = avl_tree_new(&float_compare);
    avl_tree_insert(tree, nodes, start);

    for(i = 0; i < g->vertexNum; i++) {
        minNode = avl_tree_root_node(tree);
        if(minNode == NULL)
            break;
        /*获取最小值,todo:avl tree中记录,提高效率*/
        while(avl_tree_node_child(minNode, AVL_TREE_NODE_LEFT))
            minNode = avl_tree_node_child(minNode, AVL_TREE_NODE_LEFT);
        minVertex = ((DijkstraPathNode * )avl_tree_node_key(minNode))->vertex;

        ListIterator iter;
        list_iterate(&minVertex->edges, &iter);
        while (list_iter_has_more(&iter)) {
            edge = list_iter_next(&iter);
            /*根据边寻找下一个顶点*/
            if(g->graphType == GRAPH_UNDIRECTED) {
                if(edge->vertices[0] == minVertex)
                    adjacentVertex = edge->vertices[1];
                else
                    adjacentVertex = edge->vertices[0];
            } else
                adjacentVertex = edge->vertices[1];
            /*如果该节点已经处理过, todo: 效率不高，用hashmap重写*/
            isHandled = 0;
            for(j = 0; j < i; j++) {
                if(*(path->vertexIds + j) == adjacentVertex->id) {
                    isHandled = 1;
                    break;
                }
            }
            if(isHandled)
                continue;
            /*更新*/
            newCost = ((DijkstraPathNode * )avl_tree_node_key(minNode))->cost + edge->weight;
            if(newCost < *(path->pathCosts + adjacentVertex->id - 1)) {
                if (*(path->pathCosts + adjacentVertex->id - 1) < FLT_MAX) {
                    //todo:删除操作在不同节点却cost相同的情况下会出错,需要在avl中增加另外的方法
                    avl_tree_remove(tree, nodes + adjacentVertex->id - 1);
                }
                /* re-insert on candidate tree with the better metric */
                *(path->pathCosts + adjacentVertex->id - 1) = newCost;
                (nodes + adjacentVertex->id - 1)->vertex = adjacentVertex;
                (nodes + adjacentVertex->id - 1)->cost = newCost;
                avl_tree_insert(tree, nodes + adjacentVertex->id - 1, adjacentVertex);
            }
        }

        //把新加入的节点的最短路径值和
        *(path->pathCosts + minVertex->id - 1) = ((DijkstraPathNode * )avl_tree_node_key(minNode))->cost;
        *(path->vertexIds + i) = minVertex->id;
        avl_tree_remove(tree, nodes + minVertex->id - 1);
	}
 	free(nodes);
	avl_tree_free(tree);
	return path;
}

static void graph_free(Graph * g) {
    /*清理顶点和边的内存*/
    ListIterator iter;
    list_iterate(&g->vertices, &iter);
    while (list_iter_has_more(&iter)) {
        Vertex * vertex = list_iter_next(&iter);
        list_free(vertex->edges);
        free(vertex);
    }

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
    g->prim_spanning_tree = &graph_prim_spanning_tree;
    g->kruskal_spanning_tree = &graph_kruskal_spanning_tree;
    g->dijkstra = &graph_dijkstra_avl;
    g->floyd = &graph_floyd;
}

//todo: 没有考虑是否有环路
int critical_path_method(Graph * g, Vertex * start, float * earlyTime, float * lateTime) {
    if(g->graphType != GRAPH_DIRECTED)
        return 0;
    Vertex * neighbor;
    Vertex * currentNode;
    Edge * edge;
    int i;

    char visited[g->vertexNum];
    for(i = 0; i < g->vertexNum; i++) {
        visited[i] = '0';
        *(earlyTime + i) = 0;
        *(lateTime + i) = FLT_MAX;
    }

    Queue * queue = queue_new();
    queue_push_tail(queue, start);
    //赋初值
    for(i = 0; i < g->vertexNum; i++)
    while(!queue_is_empty(queue)) {
        currentNode = (Vertex *)queue_pop_head(queue);
        visited[currentNode->id - 1] = '2';

        //处理与当前顶点连接的顶点
        ListIterator iter;
        list_iterate(&currentNode->edges, &iter);
        while (list_iter_has_more(&iter)) {
            edge = list_iter_next(&iter);
            //根据边寻找下一个顶点
            neighbor = edge->vertices[1];
            //更新最早发生时间
            *(earlyTime + neighbor->id - 1) = max(*(earlyTime + neighbor->id - 1), *(earlyTime + currentNode->id - 1) + edge->weight);
            //已经在队列中
            if(visited[neighbor->id - 1] != '0')
               continue;
            visited[neighbor->id - 1] = '1';
            queue_push_tail(queue, neighbor);
        }
    }
    //从最后一个节点开始回溯
    for(i = 0; i < g->vertexNum; i++)
        visited[i] = '0';
    *(lateTime + currentNode->id - 1) = *(earlyTime + currentNode->id - 1);
    queue_push_tail(queue, currentNode);
    while(!queue_is_empty(queue)) {
        currentNode = (Vertex *)queue_pop_head(queue);
        visited[currentNode->id - 1] = '2';

        //todo:效率不高
        for(i = 0; i < g->edgeNum; i++){
            if(((Edge *)list_nth_data(g->edges, i))->vertices[1] != currentNode)
                continue;
            edge = list_nth_data(g->edges, i);
            //根据边寻找下一个顶点
            neighbor = edge->vertices[0];
            //更新最早发生时间
            *(lateTime + neighbor->id - 1) = min(*(lateTime + neighbor->id - 1), *(lateTime + currentNode->id - 1) - edge->weight);
            //已经在队列中
            if(visited[neighbor->id - 1] != '0')
               continue;
            visited[neighbor->id - 1] = '1';
            queue_push_tail(queue, neighbor);
        }
    }
    queue_free(queue);
    return 1;
}