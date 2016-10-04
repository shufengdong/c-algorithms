/*

Copyright (c) 2005-2008, Simon Howard

Permission to use, copy, modify, and/or distribute this software
for any purpose with or without fee is hereby granted, provided
that the above copyright notice and this permission notice appear
in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

 */

/** @file avl-tree.h
 *
 * @brief Balanced binary tree
 *
 * The AVL tree structure is a balanced binary tree which stores
 * a collection of nodes (see @ref AVLTreeNode).  Each node has
 * a key and a value associated with it.  The nodes are sorted
 * within the tree based on the order of their keys. Modifications
 * to the tree are constructed such that the tree remains
 * balanced at all times (there are always roughly equal numbers
 * of nodes on either side of the tree).
 *
 * Balanced binary trees have several uses.  They can be used
 * as a mapping (searching for a value based on its key), or
 * as a set of keys which is always ordered.
 *
 * To create a new AVL tree, use @ref avl_tree_new.  To destroy
 * an AVL tree, use @ref avl_tree_free.
 *
 * To insert a new key-value pair into an AVL tree, use
 * @ref avl_tree_insert.  To remove an entry from an
 * AVL tree, use @ref avl_tree_remove or @ref avl_tree_remove_node.
 *
 * To search an AVL tree, use @ref avl_tree_lookup or
 * @ref avl_tree_lookup_node.
 *
 * Tree nodes can be queried using the
 * @ref avl_tree_node_child,
 * @ref avl_tree_node_parent,
 * @ref avl_tree_node_key and
 * @ref avl_tree_node_value functions.
 */

#ifndef ALGORITHM_AVLTREE_H
#define ALGORITHM_AVLTREE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 二叉平衡树结构
 *
 * @see avl_tree_new
 */

typedef struct _AVLTree AVLTree;

/**
 * A key for an @ref AVLTree.
 */
/* 二叉平衡树的关键字 */
typedef void *AVLTreeKey;

/**
 * A value stored in an @ref AVLTree.
 */

typedef void *AVLTreeValue;

/**
 * A null @ref AVLTreeValue.
 */
/* 二叉平衡树的空指针数据 */
#define AVL_TREE_NULL ((void *) 0)

/**
 * 二叉平衡树节点
 *
 * @see avl_tree_node_left_child
 * @see avl_tree_node_right_child
 * @see avl_tree_node_parent
 * @see avl_tree_node_key
 * @see avl_tree_node_value
 */

typedef struct _AVLTreeNode AVLTreeNode;

/**
 * An @ref AVLTreeNode can have left and right children.
 */
/* 二叉平衡树的左、右孩子标记 */
typedef enum {
    AVL_TREE_NODE_LEFT = 0,
    AVL_TREE_NODE_RIGHT = 1
} AVLTreeNodeSide;

/**
 * 比较二叉平衡树关键字的函数
 *
 * @param value1           The first key.
 * @param value2           The second key.
 * @return                 A negative number if value1 should be sorted
 *                         before value2, a positive number if value2 should
 *                         be sorted before value1, zero if the two keys
 *                         are equal.
 */

typedef int (*AVLTreeCompareFunc)(AVLTreeValue value1, AVLTreeValue value2);

/**
 * 建立一棵新的二叉平衡树
 *
 * @param compare_func    Function to use when comparing keys in the tree.
 * @return                A new AVL tree, or NULL if it was not possible
 *                        to allocate the memory.
 */

AVLTree *avl_tree_new(AVLTreeCompareFunc compare_func);

/**
 * 销毁一棵二叉平衡树
 *
 * @param tree            The tree to destroy.
 */

void avl_tree_free(AVLTree *tree);

/**
 * 在二叉平衡树中插入一个节点
 *
 * @param tree            The tree.
 * @param key             The key to insert.
 * @param value           The value to insert.
 * @return                The newly created tree node containing the
 *                        key and value, or NULL if it was not possible
 *                        to allocate the new memory.
 */

AVLTreeNode *avl_tree_insert(AVLTree *tree, AVLTreeKey key, AVLTreeValue value);

/**
 * 删除一个给定的节点
 *
 * @param tree            The tree.
 * @param node            The node to remove
 */

void avl_tree_remove_node(AVLTree *tree, AVLTreeNode *node);

/**
 * 给定关键字，删除一个节点
 *
 * @param tree            The tree.
 * @param key             The key of the node to remove.
 * @return                Zero (false) if no node with the specified key was
 *                        found in the tree, non-zero (true) if a node with
 *                        the specified key was removed.
 */

int avl_tree_remove(AVLTree *tree, AVLTreeKey key);

/**
 * 给定关键字，查找节点并返回该节点的指针
 *
 * @param tree            The AVL tree to search.
 * @param key             The key to search for.
 * @return                The tree node containing the given key, or NULL
 *                        if no entry with the given key is found.
 */

AVLTreeNode *avl_tree_lookup_node(AVLTree *tree, AVLTreeKey key);

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
/* 给定关键字，查找节点并返回节点中的数据 */
AVLTreeValue avl_tree_lookup(AVLTree *tree, AVLTreeKey key);

/**
 * 查找二叉平衡树的根节点
 *
 * @param tree            The tree.
 * @return                The root node of the tree, or NULL if the tree is
 *                        empty.
 */

AVLTreeNode *avl_tree_root_node(AVLTree *tree);

/**
 * 获取给定节点的关键字
 *
 * @param node            The tree node.
 * @return                The key to the given node.
 */

AVLTreeKey avl_tree_node_key(AVLTreeNode *node);

/**
 * 获取给定节点的数据
 *
 * @param node            The tree node.
 * @return                The value at the given node.
 */

AVLTreeValue avl_tree_node_value(AVLTreeNode *node);

/**
 * 查找给定节点的孩子节点
 *
 * @param node            The tree node.
 * @param side            Which child node to get (left or right)
 * @return                The child of the tree node, or NULL if the
 *                        node has no child on the given side.
 */

AVLTreeNode *avl_tree_node_child(AVLTreeNode *node, AVLTreeNodeSide side);

/**
 * 查找给定节点的双亲节点
 *
 * @param node            The tree node.
 * @return                The parent node of the tree node, or NULL if
 *                        this is the root node.
 */

AVLTreeNode *avl_tree_node_parent(AVLTreeNode *node);

/**
 * 计算子树的高度
 *
 * @param node            The root node of the subtree.
 * @return                The height of the subtree.
 */

int avl_tree_subtree_height(AVLTreeNode *node);

/**
 * 将二叉平衡树中所有的关键字按中序存放在一个数组中
 *
 * @param tree            The tree.
 * @return                A newly allocated C array containing all the keys
 *                        in the tree, in order.  The length of the array
 *                        is equal to the number of entries in the tree
 *                        (see @ref avl_tree_num_entries).
 */

AVLTreeValue *avl_tree_to_array(AVLTree *tree);

/**
 * 获取二叉平衡树中的节点数
 *
 * @param tree            The tree.
 * @return                The number of key-value pairs stored in the tree.
 */

unsigned int avl_tree_num_entries(AVLTree *tree);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ALGORITHM_AVLTREE_H */

