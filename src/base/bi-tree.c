#include <stdlib.h>

#include "bi-tree.h"

/* malloc() / free() testing */

#ifdef ALLOC_TESTING
#include "alloc-testing.h"
#endif

static void bi_tree_remove_subtree(BiTree *tree, BiTreeNode *node) {
    if (node == NULL) {
        return;
    }

    bi_tree_remove_subtree(tree, node->children[BI_TREE_NODE_LEFT]);
    bi_tree_remove_subtree(tree, node->children[BI_TREE_NODE_RIGHT]);

    free(node);
    tree->nodeNum--;
}

static void bi_tree_free(BiTree *tree) {
    /* Destroy all nodes */

    bi_tree_free_subtree(tree->rootNode);

    /* Free back the main tree data structure */

    free(tree);
}

static BiTreeNode * bi_tree_insert(BiTree *tree, BiTreeValue value,
                            BiTreeNode *parent, BiTreeNodeSide side) {

    BiTreeNode * newNode;
    if((newNode = (BiTreeNode *)malloc(sizeof(BiTreeNode))) == NULL)
        return NULL;
    //初始化
    newNode->children[BI_TREE_NODE_LEFT] = NULL;
    newNode->children[BI_TREE_NODE_RIGHT] = NULL;
    newNode->value = value;
    newNode->parent = parent;

    if(parent == NULL) {
    //插入根节点
        if(tree->rootNode != NULL)
            tree->rootNode->parent = newNode;
        newNode->children[side] = tree->rootNode;
        tree->rootNode = newNode;
    } else {
        if(parent->children[side] != NULL)
            parent->children[side]->parent = newNode;
        newNode->children[side] = parent->children[side];
        parent->children[side] = newNode;
    }
    tree->nodeNum++;
    return newNode;
}

static void bi_tree_remove(BiTree *tree, BiTreeNode *node) {
    BiTreeNode *parent = node->parent;
    if(parent != NULL) {
        if(parent->children[BI_TREE_NODE_LEFT] == node) {
            parent->children[BI_TREE_NODE_LEFT] = NULL;
        } else if(parent->children[BI_TREE_NODE_RIGHT] == node) {
            parent->children[BI_TREE_NODE_RIGHT] = NULL;
        }
    } else /*这是根节点*/
        tree->rootNode = NULL;
    bi_tree_remove_subtree(tree, node);
}

static BiTreeNode *bi_tree_rootNode(BiTree *tree) {
    return tree->rootNode;
}

static BiTreeValue bi_tree_node_value(BiTreeNode *node) {
    return node->value;
}

static BiTreeNode *bi_tree_node_child(BiTreeNode *node, BiTreeNodeSide side) {
    if (side == BI_TREE_NODE_LEFT || side == BI_TREE_NODE_RIGHT) {
        return node->children[side];
    } else {
        return NULL;
    }
}

static BiTreeNode *bi_tree_node_parent(BiTreeNode *node) {
    return node->parent;
}

/* Binary Tree new */

BiTree * bi_tree_new() {
    BiTree *newTree;

    newTree = (BiTree *) malloc(sizeof(BiTree));

    if (newTree == NULL) {
        return NULL;
    }
    /*初始化二叉树*/
    bi_tree_initial(newTree);

    return newTree;
}

void bi_tree_initial(BiTree *tree) {
    if (tree == NULL)
        return;
    tree->rootNode = NULL;
    tree->nodeNum = 0;
    tree->free_tree = &bi_tree_free;
    tree->insert = &bi_tree_insert;
    tree->remove = &bi_tree_remove;
    tree->root_node = &bi_tree_rootNode;
    tree->node_value = &bi_tree_node_value;
    tree->node_child = &bi_tree_node_child;
    tree->node_parent = &bi_tree_node_parent;
}

void bi_tree_free_subtree(BiTreeNode *node) {
    if (node == NULL) {
        return;
    }

    bi_tree_free_subtree(node->children[BI_TREE_NODE_LEFT]);
    bi_tree_free_subtree(node->children[BI_TREE_NODE_RIGHT]);

    free(node);
}