
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

#include <stdlib.h>

#include "rb-tree.h"

/* malloc() / free() testing */

#ifdef ALLOC_TESTING
#include "alloc-testing.h"
#endif

struct _RBTreeNode {
	RBTreeNodeColor color;
	RBTreeKey key;
	RBTreeValue value;
	RBTreeNode *parent;
	RBTreeNode *children[2];
};

struct _RBTree {
	RBTreeNode *root_node;
	RBTreeCompareFunc compare_func;
	int num_nodes;
};

static RBTreeNodeSide rb_tree_node_side(RBTreeNode *node) {
	if (node->parent->children[RB_TREE_NODE_LEFT] == node) {
		return RB_TREE_NODE_LEFT;
	} else {
		return RB_TREE_NODE_RIGHT;
	}
}

static RBTreeNode *rb_tree_node_sibling(RBTreeNode *node) {
	RBTreeNodeSide side;

	side = rb_tree_node_side(node);

	return node->parent->children[1 - side];
}

RBTreeNode *rb_tree_node_uncle(RBTreeNode *node) {
	return rb_tree_node_sibling(node->parent);
}

/* 将节点1用节点2替换 */

static void rb_tree_node_replace(RBTree *tree, RBTreeNode *node1,
                                 RBTreeNode *node2) {
	int side;

	/* 设置节点的双亲指针 */

	if (node2 != NULL) {
		node2->parent = node1->parent;
	}

	/* 是根节点? */

	if (node1->parent == NULL) {
		tree->root_node = node2;
	} else {
		side = rb_tree_node_side(node1);
		node1->parent->children[side] = node2;
	}
}

/* Rotate a section of the tree.  'node' is the node at the top
 * of the section to be rotated.  'direction' is the direction in
 * which to rotate the tree: left or right, as shown in the following
 * diagram:
 *
 * Left rotation:              Right rotation:
 *
 *      B                             D
 *     / \                           / \
 *    A   D                         B   E
 *       / \                       / \
 *      C   E                     A   C

 * is rotated to:              is rotated to:
 *
 *        D                           B
 *       / \                         / \
 *      B   E                       A   D
 *     / \                             / \
 *    A   C                           C   E
 */
/* 单旋转。node是待旋转子树的根节点，direction是旋转的方向 */
static RBTreeNode *rb_tree_rotate(RBTree *tree, RBTreeNode *node,
                                  RBTreeNodeSide direction) {
	RBTreeNode *new_root;

	/* 根节点的孩子节点将取代其位置：左旋转则右孩子取代，反之左孩子取代 */

	new_root = node->children[1-direction];

	/* Make new_root the root, update parent pointers. */

	rb_tree_node_replace(tree, node, new_root);

	/* 重置指针变量 */

	node->children[1-direction] = new_root->children[direction];
	new_root->children[direction] = node;

	/* 更新双亲节点 */

	node->parent = new_root;

	if (node->children[1-direction] != NULL) {
		node->children[1-direction]->parent = node;
	}

	return new_root;
}


RBTree *rb_tree_new(RBTreeCompareFunc compare_func) {
	RBTree *new_tree;

	new_tree = malloc(sizeof(RBTree));

	if (new_tree == NULL) {
		return NULL;
	}

	new_tree->root_node = NULL;
	new_tree->num_nodes = 0;
	new_tree->compare_func = compare_func;

	return new_tree;
}

static void rb_tree_free_subtree(RBTreeNode *node) {
	if (node != NULL) {
		/* 递归删除孩子节点 */

		rb_tree_free_subtree(node->children[RB_TREE_NODE_LEFT]);
		rb_tree_free_subtree(node->children[RB_TREE_NODE_RIGHT]);

		/* 释放该节点内存 */

		free(node);
	}
}

void rb_tree_free(RBTree *tree) {
	/* 销毁树中所有节点 */

	rb_tree_free_subtree(tree->root_node);

	/* 释放红黑树结构内存 */

	free(tree);
}

static void rb_tree_insert_case1(RBTree *tree, RBTreeNode *node);
static void rb_tree_insert_case2(RBTree *tree, RBTreeNode *node);
static void rb_tree_insert_case3(RBTree *tree, RBTreeNode *node);
static void rb_tree_insert_case4(RBTree *tree, RBTreeNode *node);
static void rb_tree_insert_case5(RBTree *tree, RBTreeNode *node);

/* 插入情形1：新节点是根节点，则它必须涂成黑色，因为根节点总是黑色的 */

static void rb_tree_insert_case1(RBTree *tree, RBTreeNode *node) {
	if (node->parent == NULL) {

		/* 根节点是黑色的 */

		node->color = RB_TREE_NODE_BLACK;

	} else {

		/* 不是根节点 */

		rb_tree_insert_case2(tree, node);
	}
}

/* Insert case 2: If the parent of the new node is red, this
 * conflicts with the red-black tree conditions, as both children
 * of every red node are black. */
/* 插入情形2：新节点的父节点是红色的 */
static void rb_tree_insert_case2(RBTree *tree, RBTreeNode *node) {
	/* 当调用这个函数的时候，已经保证了插入的节点不是根节点 */

	if (node->parent->color != RB_TREE_NODE_BLACK) {
		rb_tree_insert_case3(tree, node);
	}
}

/* Insert case 3: If the parent and uncle are both red, repaint them
 * both black and repaint the grandparent red.  */
/* 插入情形3：如果父节点及其兄弟节点都是红色的 */
static void rb_tree_insert_case3(RBTree *tree, RBTreeNode *node) {
	RBTreeNode *grandparent;
	RBTreeNode *uncle;

	/* 因为父节点是红色的，所以插入的节点一定存在祖父节点 */

	grandparent = node->parent->parent;
	uncle = rb_tree_node_uncle(node);

	if (uncle != NULL && uncle->color == RB_TREE_NODE_RED) {

		node->parent->color = RB_TREE_NODE_BLACK;
		uncle->color = RB_TREE_NODE_BLACK;
		grandparent->color = RB_TREE_NODE_RED;

		/* 递归上滤直到不再有两个相连的红色节点或到达根处 */

		rb_tree_insert_case1(tree, grandparent);

	}
	else {
		rb_tree_insert_case4(tree, node);
	}
}

/* Case 4: If the parent is red, but the uncle is black, we need to do
 * some rotations to keep the tree balanced and complying with the tree
 * conditions.  If the node is on the opposite side relative to its parent
 * as the parent is relative to its grandparent, rotate around the
 * parent.  Either way, we will continue to case 5.
 *
 * eg.
 *
 *         B                              B
 *        / \                            / \
 *       R   B          ->     node ->  R   B
 *        \                            /
 *         R  <- node                 R
 *
 */
/* 插入情形4：父节点是红色的，父节点的兄弟节点是黑色的 */
void rb_tree_insert_case4(RBTree *tree, RBTreeNode *node)
{
	RBTreeNode *next_node;
	RBTreeNodeSide side;

	/* Note that at this point, by implication from case 3, we know
	 * that the parent is red, but the uncle is black.  We therefore
	 * only need to examine what side the node is on relative
	 * to its parent, and the side the parent is on relative to
	 * the grandparent. */

	side = rb_tree_node_side(node);

	if (side != rb_tree_node_side(node->parent)) {

		/* After the rotation, we will continue to case 5, but
		 * the parent node will be at the bottom. */
        /* 需要进行双旋转。这里旋转一次后成为情形5 */
		next_node = node->parent;

		/* 按插入节点的左右方向的相反方向旋转父节点 */

		rb_tree_rotate(tree, node->parent, 1-side);
	} else {
		next_node = node;
	}

	rb_tree_insert_case5(tree, next_node);
}

/* Case 5: The node is on the same side relative to its parent as the
 * parent is relative to its grandparent.  The node and its parent are
 * red, but the uncle is black.
 *
 * Now, rotate at the grandparent and recolor the parent and grandparent
 * to black and red respectively.
 *
 *               G/B                 P/B
 *              /   \               /   \
 *           P/R     U/B    ->   N/R     G/R
 *          /   \                       /   \
 *       N/R      ?                   ?      U/B
 *
 */
/* 插入情形5：插入的节点及其父节点作为作为孩子节点的左右方向相同，都为红色，父节点的兄弟节点为黑色 */
void rb_tree_insert_case5(RBTree *tree, RBTreeNode *node) {
	RBTreeNode *parent;
	RBTreeNode *grandparent;
	RBTreeNodeSide side;

	parent = node->parent;
	grandparent = parent->parent;

	/* What side are we, relative to the parent?  This will determine
	 * the direction that we rotate. */
    /* 插入节点的左右方向 */
	side = rb_tree_node_side(node);

	/* Rotate at the grandparent, in the opposite direction to side. */
    /* 按相反方向旋转祖父节点 */
	rb_tree_rotate(tree, grandparent, 1-side);

	/* 重新着色 */

	parent->color = RB_TREE_NODE_BLACK;
	grandparent->color = RB_TREE_NODE_RED;
}

RBTreeNode *rb_tree_insert(RBTree *tree, RBTreeKey key, RBTreeValue value) {
	RBTreeNode *node;
	RBTreeNode **rover;
	RBTreeNode *parent;
	RBTreeNodeSide side;

	/* 申请新节点空间 */

	node = malloc(sizeof(RBTreeNode));

	if (node == NULL) {
		return NULL;
	}

	/* 初始化新节点。涂成红色 */

	node->key = key;
	node->value = value;
	node->color = RB_TREE_NODE_RED;
	node->children[RB_TREE_NODE_LEFT] = NULL;
	node->children[RB_TREE_NODE_RIGHT] = NULL;

	/* 首先进行正常的二叉平衡树插入操作 */

	parent = NULL;
	rover = &tree->root_node;

	while (*rover != NULL) {

		/* 更新父节点指针 */

		parent = *rover;

		/* 选择搜索路径的方向，向左孩子或右孩子 */

		if (tree->compare_func(value, (*rover)->value) < 0) {
			side = RB_TREE_NODE_LEFT;
		} else {
			side = RB_TREE_NODE_RIGHT;
		}

		rover = &(*rover)->children[side];
	}

	/* 在搜索到达的位置插入节点 */
	*rover = node;
	node->parent = parent;

	/* 调整树以满足红黑树性质 */
	rb_tree_insert_case1(tree, node);

	/* 更新节点数 */
	++tree->num_nodes;

	return node;
}

RBTreeNode *rb_tree_lookup_node(RBTree *tree, RBTreeKey key) {
	RBTreeNode *node;
	RBTreeNodeSide side;
	int diff;

	node = tree->root_node;

	/* 搜索这棵树 */

	while (node != NULL) {
		diff = tree->compare_func(key, node->key);

		if (diff == 0) {
			return node;
		}
		else if (diff < 0) {
			side = RB_TREE_NODE_LEFT;
		}
		else {
			side = RB_TREE_NODE_RIGHT;
		}
		node = node->children[side];
	}

	/* 未找到 */
	return NULL;
}

RBTreeValue rb_tree_lookup(RBTree *tree, RBTreeKey key) {
	RBTreeNode *node;

	/* 查找具有该关键字的节点 */
	node = rb_tree_lookup_node(tree, key);

	if (node == NULL) {
		return RB_TREE_NULL;
	}
	else {
		return node->value;
	}
}

void rb_tree_remove_node(RBTree *tree, RBTreeNode *node) {
	/* TODO */
}

int rb_tree_remove(RBTree *tree, RBTreeKey key) {
	RBTreeNode *node;

	/* 查找待删除节点 */
	node = rb_tree_lookup_node(tree, key);

	if (node == NULL) {
		return 0;
	}
	rb_tree_remove_node(tree, node);
	return 1;
}

RBTreeNode *rb_tree_root_node(RBTree *tree) {
	return tree->root_node;
}

RBTreeKey rb_tree_node_key(RBTreeNode *node) {
	return node->key;
}

RBTreeValue rb_tree_node_value(RBTreeNode *node) {
	return node->value;
}

RBTreeNode *rb_tree_node_child(RBTreeNode *node, RBTreeNodeSide side) {
	if (side == RB_TREE_NODE_LEFT || side == RB_TREE_NODE_RIGHT) {
		return node->children[side];
	}
	else {
		return NULL;
	}
}

RBTreeNode *rb_tree_node_parent(RBTreeNode *node) {
	return node->parent;
}

RBTreeValue *rb_tree_to_array(RBTree *tree) {
	/* TODO */
	return NULL;
}

int rb_tree_num_entries(RBTree *tree) {
	return tree->num_nodes;
}

