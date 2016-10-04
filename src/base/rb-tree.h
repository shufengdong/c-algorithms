/*

Copyright (c) 2008, Simon Howard

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

/** @file rbtree.h
 *
 * @brief Balanced binary tree
 *
 * The red-black tree structure is a balanced binary tree which stores
 * a collection of nodes (see @ref RBTreeNode).  Each node has
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
 * To create a new red-black tree, use @ref rb_tree_new.  To destroy
 * a red-black tree, use @ref rb_tree_free.
 *
 * To insert a new key-value pair into a red-black tree, use
 * @ref rb_tree_insert.  To remove an entry from a
 * red-black tree, use @ref rb_tree_remove or @ref rb_tree_remove_node.
 *
 * To search a red-black tree, use @ref rb_tree_lookup or
 * @ref rb_tree_lookup_node.
 *
 * Tree nodes can be queried using the
 * @ref rb_tree_node_left_child,
 * @ref rb_tree_node_right_child,
 * @ref rb_tree_node_parent,
 * @ref rb_tree_node_key and
 * @ref rb_tree_node_value functions.
 */

#ifndef ALGORITHM_RB_TREE_H
#define ALGORITHM_RB_TREE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 红黑树
 *
 * @see rb_tree_new
 */

typedef struct _RBTree RBTree;

/**
 * A key for an @ref RBTree.
 */
/* 红黑树关键字 */
typedef void *RBTreeKey;

/**
 * A value stored in an @ref RBTree.
 */
/* 红黑树中存储的数据 */
typedef void *RBTreeValue;

/**
 * A null @ref RBTreeValue.
 */
/* 红黑树的空指针数据 */
#define RB_TREE_NULL ((void *) 0)

/**
 * 红黑树节点
 *
 * @see rb_tree_node_left_child
 * @see rb_tree_node_right_child
 * @see rb_tree_node_parent
 * @see rb_tree_node_key
 * @see rb_tree_node_value
 */

typedef struct _RBTreeNode RBTreeNode;

/**
 * 比较红黑树关键字的函数
 *
 * @param data1            The first key.
 * @param data2            The second key.
 * @return                 A negative number if data1 should be sorted
 *                         before data2, a positive number if data2 should
 *                         be sorted before data1, zero if the two keys
 *                         are equal.
 */

typedef int (*RBTreeCompareFunc)(RBTreeValue data1, RBTreeValue data2);

/**
 * 红黑树节点的红黑颜色标记
 */

typedef enum {
	RB_TREE_NODE_RED,
	RB_TREE_NODE_BLACK,
} RBTreeNodeColor;

/**
 * A @ref RBTreeNode can have left and right children.
 */
/* 红黑树的左、右孩子标记 */
typedef enum {
	RB_TREE_NODE_LEFT = 0,
	RB_TREE_NODE_RIGHT = 1
} RBTreeNodeSide;

/**
 * 建立一棵新的红黑树
 *
 * @param compare_func    Function to use when comparing keys in the tree.
 * @return                A new red-black tree, or NULL if it was not possible
 *                        to allocate the memory.
 */

RBTree *rb_tree_new(RBTreeCompareFunc compare_func);

/**
 * 销毁一棵红黑树
 *
 * @param tree            The tree to destroy.
 */

void rb_tree_free(RBTree *tree);

/**
 * 在红黑树中插入一个节点
 *
 * @param tree            The tree.
 * @param key             The key to insert.
 * @param value           The value to insert.
 * @return                The newly created tree node containing the
 *                        key and value, or NULL if it was not possible
 *                        to allocate the new memory.
 */

RBTreeNode *rb_tree_insert(RBTree *tree, RBTreeKey key, RBTreeValue value);

/**
 * 删除一个给定的节点
 *
 * @param tree            The tree.
 * @param node            The node to remove
 */

void rb_tree_remove_node(RBTree *tree, RBTreeNode *node);

/**
 * 给定关键字，删除一个节点
 *
 * @param tree            The tree.
 * @param key             The key of the node to remove.
 * @return                Zero (false) if no node with the specified key was
 *                        found in the tree, non-zero (true) if a node with
 *                        the specified key was removed.
 */

int rb_tree_remove(RBTree *tree, RBTreeKey key);

/**
 * 给定关键字，查找节点并返回该节点的指针
 *
 * @param tree            The red-black tree to search.
 * @param key             The key to search for.
 * @return                The tree node containing the given key, or NULL
 *                        if no entry with the given key is found.
 */

RBTreeNode *rb_tree_lookup_node(RBTree *tree, RBTreeKey key);

/**
 * Search a red-black tree for a value corresponding to a particular key.
 * This uses the tree as a mapping.  Note that this performs
 * identically to @ref rb_tree_lookup_node, except that the value
 * at the node is returned rather than the node itself.
 *
 * @param tree            The red-black tree to search.
 * @param key             The key to search for.
 * @return                The value associated with the given key, or
 *                        RB_TREE_NULL if no entry with the given key is
 *                        found.
 */
/* 给定关键字，查找节点并返回节点中的数据 */
RBTreeValue rb_tree_lookup(RBTree *tree, RBTreeKey key);

/**
 * 查找红黑树的根节点
 *
 * @param tree            The tree.
 * @return                The root node of the tree, or NULL if the tree is
 *                        empty.
 */

RBTreeNode *rb_tree_root_node(RBTree *tree);

/**
 * 获取给定节点的关键字
 *
 * @param node            The tree node.
 * @return                The key to the given node.
 */

RBTreeKey rb_tree_node_key(RBTreeNode *node);

/**
 * 获取给定节点的数据
 *
 * @param node            The tree node.
 * @return                The value at the given node.
 */

RBTreeValue rb_tree_node_value(RBTreeNode *node);

/**
 * 查找给定节点的孩子节点
 *
 * @param node            The tree node.
 * @param side            The side relative to the node.
 * @return                The child of the tree node, or NULL if the
 *                        node has no child on the specified side.
 */

RBTreeNode *rb_tree_node_child(RBTreeNode *node, RBTreeNodeSide side);

/**
 * 查找给定节点的双亲节点
 *
 * @param node            The tree node.
 * @return                The parent node of the tree node, or NULL if
 *                        this is the root node.
 */

RBTreeNode *rb_tree_node_parent(RBTreeNode *node);

/**
 * 计算子树的高度
 *
 * @param node            The root node of the subtree.
 * @return                The height of the subtree.
 */

int rb_tree_subtree_height(RBTreeNode *node);

/**
 * 将红黑树中所有的关键字按中序存放在一个数组中
 *
 * @param tree            The tree.
 * @return                A newly allocated C array containing all the keys
 *                        in the tree, in order.  The length of the array
 *                        is equal to the number of entries in the tree
 *                        (see @ref rb_tree_num_entries).
 */

RBTreeValue *rb_tree_to_array(RBTree *tree);

/**
 * 获取红黑树中的节点数
 *
 * @param tree            The tree.
 * @return                The number of key-value pairs stored in the tree.
 */

int rb_tree_num_entries(RBTree *tree);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ALGORITHM_RB_TREE_H */

