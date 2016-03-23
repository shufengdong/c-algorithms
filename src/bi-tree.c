#include <stdlib.h>

#include "bi-tree.h"

/* malloc() / free() testing */

#ifdef ALLOC_TESTING
#include "alloc-testing.h"
#endif


/* Binary Tree methods */

BiTree * bi_tree_new() {
    BiTree *new_tree;

    new_tree = (BiTree *) malloc(sizeof(BiTree));

    if (new_tree == NULL) {
        return NULL;
    }

    new_tree->root_node = NULL;
    new_tree->num_nodes = 0;

    return new_tree;
}

static void bi_tree_free_subtree(BiTree *tree, BiTreeNode *node) {
    if (node == NULL) {
        return;
    }

    bi_tree_free_subtree(tree, node->children[BI_TREE_NODE_LEFT]);
    bi_tree_free_subtree(tree, node->children[BI_TREE_NODE_RIGHT]);

    free(node);
    tree->num_nodes--;
}

void bi_tree_free(BiTree *tree) {
    /* Destroy all nodes */

    bi_tree_free_subtree(tree, tree->root_node);

    /* Free back the main tree data structure */

    free(tree);
}

BiTreeNode * bi_tree_insert(BiTree *tree, BiTreeValue value,
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
        if(tree->root_node != NULL)
            tree->root_node->parent = newNode;
        newNode->children[side] = tree->root_node;
        tree->root_node = newNode;
    } else {
        if(parent->children[side] != NULL)
            parent->children[side]->parent = newNode;
        newNode->children[side] = parent->children[side];
        parent->children[side] = newNode;
    }
    tree->num_nodes++;
    return newNode;
}

void bi_tree_remove_node(BiTree *tree, BiTreeNode *node) {
    BiTreeNode *parent = node->parent;
    if(parent == NULL)
        return;
    if(parent->children[BI_TREE_NODE_LEFT] == node) {
        parent->children[BI_TREE_NODE_LEFT] = NULL;
    } else if(parent->children[BI_TREE_NODE_RIGHT] == node) {
        parent->children[BI_TREE_NODE_RIGHT] = NULL;
    }
    bi_tree_free_subtree(tree, node);
}

BiTreeNode *bi_tree_root_node(BiTree *tree) {
    return tree->root_node;
}

BiTreeValue bi_tree_node_value(BiTreeNode *node) {
    return node->value;
}

BiTreeNode *bi_tree_node_child(BiTreeNode *node, BiTreeNodeSide side) {
    if (side == BI_TREE_NODE_LEFT || side == BI_TREE_NODE_RIGHT) {
        return node->children[side];
    } else {
        return NULL;
    }
}

BiTreeNode *bi_tree_node_parent(BiTreeNode *node) {
    return node->parent;
}