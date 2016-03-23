#ifndef ALGORITHM_BITREE_H
#define ALGORITHM_BITREE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BI_TREE_NODE_LEFT = 0,
    BI_TREE_NODE_RIGHT = 1
} BiTreeNodeSide;

/**
 * An binary tree
 *
 * @see bi_tree_new
 */

typedef struct _BiTree BiTree;

/**
 * A node in an binary tree.
 */

typedef struct _BiTreeNode BiTreeNode;

/**
 * A value stored in an @ref binary Tree.
 */

typedef void *BiTreeValue;

struct _BiTreeNode {
    BiTreeNode *children[2];
    BiTreeNode *parent;
    BiTreeValue value;
};

struct _BiTree {
    BiTreeNode *root_node;
    unsigned int num_nodes;
};


/**
 * A null @ref BiTreeValue.
 */

#define BI_TREE_NULL ((void *) 0)


/**
 * Create a new binary tree.
 *
 * @param compare_func    Function to use when comparing keys in the tree.
 * @return                A new binary tree, or NULL if it was not possible
 *                        to allocate the memory.
 */

BiTree *bi_tree_new();

/**
 * Destroy an binary tree.
 *
 * @param tree            The tree to destroy.
 */

void bi_tree_free(BiTree *tree);

/**
 * Insert a new key-value pair into an binary tree.
 *
 * @param tree            The tree.
 * @param value           The value to insert.
 * @param parent          The parent where to insert.
 * @param side            Insert to left or right.
 * @return                The newly created tree node containing the
 *                        key and value, or NULL if it was not possible
 *                        to allocate the new memory.
 */

BiTreeNode * bi_tree_insert(BiTree *tree, BiTreeValue value, BiTreeNode *parent, BiTreeNodeSide side);

/**
 * Remove a node from a tree.
 *
 * @param tree            The tree.
 * @param node            The node to remove
 */

void bi_tree_remove_node(BiTree *tree, BiTreeNode *node);


/**
 * Find the root node of a tree.
 *
 * @param tree            The tree.
 * @return                The root node of the tree, or NULL if the tree is
 *                        empty.
 */

BiTreeNode *bi_tree_root_node(BiTree *tree);


/**
 * Retrieve the value at a given tree node.
 *
 * @param node            The tree node.
 * @return                The value at the given node.
 */

BiTreeValue bi_tree_node_value(BiTreeNode *node);

/**
 * Find the child of a given tree node.
 *
 * @param node            The tree node.
 * @param side            Which child node to get (left or right)
 * @return                The child of the tree node, or NULL if the
 *                        node has no child on the given side.
 */

BiTreeNode *bi_tree_node_child(BiTreeNode *node, BiTreeNodeSide side);

/**
 * Find the parent node of a given tree node.
 *
 * @param node            The tree node.
 * @return                The parent node of the tree node, or NULL if
 *                        this is the root node.
 */

BiTreeNode *bi_tree_node_parent(BiTreeNode *node);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ALGORITHM_BITREE_H */