#ifndef ALGORITHM_BSTREE_H
#define ALGORITHM_BSTREE_H

#include "bi-tree.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * A key for an @ref BsTree.
 */

typedef void *BsTreeKey;

/**
 * An binary sort tree
 *
 * @see bs_tree_new
 */

typedef struct _BsTree BsTree;

typedef struct _BsTreeNode BsTreeNode;


struct _BsTreeNode {
    struct _BiTreeNode base;
    BsTreeKey key;
};

/**
 * Type of function used to compare keys in an binary sort tree.
 *
 * @param value1           The first key.
 * @param value2           The second key.
 * @return                 A negative number if value1 should be sorted
 *                         before value2, a positive number if value2 should
 *                         be sorted before value1, zero if the two keys
 *                         are equal.
 */

typedef int (*BsTreeCompareFunc)(BiTreeValue value1, BiTreeValue value2);

struct _BsTree {
    /* base  */
    struct _BiTree base;
    /*查找时比较两个key时用的函数*/
    BsTreeCompareFunc compareFunc;


    /**
     * Insert a new key-value pair into an AVL tree.
     *
     * @param tree            The tree.
     * @param key             The key to insert.
     * @param value           The value to insert.
     * @return                The newly created tree node containing the
     *                        key and value, or NULL if it was not possible
     *                        to allocate the new memory.
     */

    BiTreeNode * (*insert_key_value)(BsTree *tree, BsTreeKey key, BiTreeValue value);

    /**
     * Remove a node from a tree.
     *
     * @param tree            The tree.
     * @param node            The node to remove
     */

    void (*remove_node)(BsTree *tree, BsTreeNode *node);


    /**
     * Remove an entry from a tree, specifying the key of the node to
     * remove.
     *
     * @param tree            The tree.
     * @param key             The key of the node to remove.
     * @return                Zero (false) if no node with the specified key was
     *                        found in the tree, non-zero (true) if a node with
     *                        the specified key was removed.
     */

    int (*remove_by_key)(BsTree *_this, BsTreeKey key);

    /**
     * Search an AVL tree for a node with a particular key.  This uses
     * the tree as a mapping.
     *
     * @param tree            The AVL tree to search.
     * @param key             The key to search for.
     * @return                The tree node containing the given key, or NULL
     *                        if no entry with the given key is found.
     */

    BsTreeNode * (*lookup_node)(BsTree *_this, BsTreeKey key);

    /**
     * Search an AVL tree for a value corresponding to a particular key.
     * This uses the tree as a mapping.  Note that this performs
     * identically to @ref avl_tree_lookup_node, except that the value
     * at the node is returned rather than the node itself.
     *
     * @param tree            The AVL tree to search.
     * @param key             The key to search for.
     * @return                The value associated with the given key, or
     *                        @ref AVL_TREE_NULL if no entry with the given key is
     *                        found.
     */

    BiTreeValue (*lookup_value)(BsTree *_this, BsTreeKey key);
};


/**
 * Create a new binary sort tree.
 *
 * @param compare_func    Function to use when comparing keys in the tree.
 * @return                A new binary sort tree, or NULL if it was not possible
 *                        to allocate the memory.
 */

BsTree *bs_tree_new(BsTreeCompareFunc compareFunc);

void bs_tree_initial(BsTree * tree, BsTreeCompareFunc compareFunc);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ALGORITHM_BITREE_H */