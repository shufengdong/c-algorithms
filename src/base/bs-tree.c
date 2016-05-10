#include <stdlib.h>

#include "bi-tree.h"
#include "bs-tree.h"

/* malloc() / free() testing */

#ifdef ALLOC_TESTING
#include "alloc-testing.h"
#endif

static BsTreeNode *bs_tree_insert(BsTree *tree, BsTreeKey key, BiTreeValue value) {
    BsTreeNode **rover;
    BsTreeNode *newNode;
    BsTreeNode *previousNode;

    /* Walk down the tree until we reach a NULL pointer */

    rover = &(((BiTree *)tree)->rootNode);
    previousNode = NULL;

    while (*rover != NULL) {
        previousNode = *rover;
        if (tree->compareFunc(key, (*rover)->key) < 0) {
            rover = &(((BiTreeNode *)(*rover))->children[BI_TREE_NODE_LEFT]);
        } else {
            rover = &(((BiTreeNode *)(*rover))->children[BI_TREE_NODE_RIGHT]);
        }
    }

    /* Create a new node.  Use the last node visited as the parent link. */

    newNode = (BsTreeNode *) malloc(sizeof(BsTreeNode));

    if (newNode == NULL) {
        return NULL;
    }

    ((BiTreeNode *)newNode)->children[BI_TREE_NODE_LEFT] = NULL;
    ((BiTreeNode *)newNode)->children[BI_TREE_NODE_RIGHT] = NULL;
    ((BiTreeNode *)newNode)->parent = previousNode;
    ((BiTreeNode *)newNode)->value = value;
    newNode->key = key;

    /* Insert at the NULL pointer that was reached */

    *rover = newNode;

    /* Keep track of the number of entries */

    ++((BiTree *)tree)->nodeNum;

    return newNode;
}


static void bs_tree_remove_node(BsTree *_this, BsTreeNode * node) {
    //todo
}

static int bs_tree_remove_by_key(BsTree *_this, BsTreeKey key) {
    return 0; //todo
}

static BsTreeNode * bs_tree_lookup_node(BsTree *tree, BsTreeKey key) {
    BsTreeNode *node;
    int diff;

    /* Search down the tree and attempt to find the node which
     * has the specified key */

    node = ((BiTree *)tree)->rootNode;

    while (node != NULL) {

        diff = tree->compareFunc(key, node->key);

        if (diff == 0) {

            /* Keys are equal: return this node */

            return node;

        } else if (diff < 0) {
            node = ((BiTreeNode *)node)->children[BI_TREE_NODE_LEFT];
        } else {
            node = ((BiTreeNode *)node)->children[BI_TREE_NODE_RIGHT];
        }
    }

    /* Not found */

    return NULL;
}

BiTreeValue bs_tree_lookup(BsTree *tree, BsTreeKey key) {
    BsTreeNode *node;

    /* Find the node */

    node = bs_tree_lookup_node(tree, key);

    if (node == NULL) {
        return BI_TREE_NULL;
    } else {
        return ((BiTreeNode *)node)->value;
    }
}


/* Binary Tree new */

BsTree * bs_tree_new(BsTreeCompareFunc compareFunc) {
    BsTree *newTree;

    newTree = (BsTree *) malloc(sizeof(BsTree));

    if (newTree == NULL) {
        return NULL;
    }
    bs_tree_initial(newTree, compareFunc);
    return newTree;
}

void bs_tree_initial(BsTree * tree, BsTreeCompareFunc compareFunc) {
    bi_tree_initial((BiTree *)tree);
    tree->compareFunc = compareFunc;
    tree->lookup_node = &bs_tree_lookup_node;
    tree->lookup_value = &bs_tree_lookup;
    tree->insert_key_value = &bs_tree_insert;
    tree->remove_by_key = &bs_tree_remove_by_key;
    ((BiTree*)tree)->insert = NULL;
    ((BiTree*)tree)->remove = &bs_tree_remove_node;
}