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

#include <stdlib.h>

#include "avl-tree.h"

/* malloc() / free() testing */

#ifdef ALLOC_TESTING
#include "alloc-testing.h"
#endif

/* 二叉平衡树节点结构 */

struct _AVLTreeNode {
    AVLTreeNode *children[2];
    AVLTreeNode *parent;
    AVLTreeKey key;
    AVLTreeValue value;
    int height;
};

struct _AVLTree {
    AVLTreeNode *root_node;
    AVLTreeCompareFunc compare_func;
    unsigned int num_nodes;
};

AVLTree *avl_tree_new(AVLTreeCompareFunc compare_func) {
    AVLTree *new_tree;

    new_tree = (AVLTree *) malloc(sizeof(AVLTree));

    if (new_tree == NULL) {
        return NULL;
    }

    new_tree->root_node = NULL;
    new_tree->compare_func = compare_func;
    new_tree->num_nodes = 0;

    return new_tree;
}

static void avl_tree_free_subtree(AVLTree *tree, AVLTreeNode *node) {
    if (node == NULL) {
        return;
    }

    avl_tree_free_subtree(tree, node->children[AVL_TREE_NODE_LEFT]);
    avl_tree_free_subtree(tree, node->children[AVL_TREE_NODE_RIGHT]);

    free(node);
}

void avl_tree_free(AVLTree *tree) {
    /* 销毁所有节点 */

    avl_tree_free_subtree(tree, tree->root_node);

    /* 释放二叉平衡树结构内存 */

    free(tree);
}

int avl_tree_subtree_height(AVLTreeNode *node) {
    if (node == NULL) {
        return 0;
    } else {
        return node->height;
    }
}

/* 更新节点的高度 */

static void avl_tree_update_height(AVLTreeNode *node) {
    AVLTreeNode *left_subtree;
    AVLTreeNode *right_subtree;
    int left_height, right_height;

    left_subtree = node->children[AVL_TREE_NODE_LEFT];
    right_subtree = node->children[AVL_TREE_NODE_RIGHT];
    left_height = avl_tree_subtree_height(left_subtree);
    right_height = avl_tree_subtree_height(right_subtree);

    if (left_height > right_height) {
        node->height = left_height + 1;
    } else {
        node->height = right_height + 1;
    }
}

/* 找出节点连在其双亲节点的哪一侧 */

static AVLTreeNodeSide avl_tree_node_parent_side(AVLTreeNode *node) {
    if (node->parent->children[AVL_TREE_NODE_LEFT] == node) {
        return AVL_TREE_NODE_LEFT;
    } else {
        return AVL_TREE_NODE_RIGHT;
    }
}

/* 将节点1用节点2替换 */

static void avl_tree_node_replace(AVLTree *tree, AVLTreeNode *node1,
                                  AVLTreeNode *node2) {
    int side;

    /* 设置节点的双亲指针 */

    if (node2 != NULL) {
        node2->parent = node1->parent;
    }

    /* 是根节点? */

    if (node1->parent == NULL) {
        tree->root_node = node2;
    } else {
        side = avl_tree_node_parent_side(node1);
        node1->parent->children[side] = node2;

        avl_tree_update_height(node1->parent);
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
static AVLTreeNode *avl_tree_rotate(AVLTree *tree, AVLTreeNode *node,
                                    AVLTreeNodeSide direction) {
    AVLTreeNode *new_root;

    /* 根节点的孩子节点将取代其位置：左旋转则右孩子取代，反之左孩子取代 */

    new_root = node->children[1-direction];

    /* Make new_root the root, update parent pointers. */

    avl_tree_node_replace(tree, node, new_root);

    /* 重置指针变量 */

    node->children[1-direction] = new_root->children[direction];
    new_root->children[direction] = node;

    /* 更新双亲节点 */

    node->parent = new_root;

    if (node->children[1-direction] != NULL) {
        node->children[1-direction]->parent = node;
    }

    /* 更新节点的高度 */

    avl_tree_update_height(new_root);
    avl_tree_update_height(node);

    return new_root;
}


/* 平衡一个节点
 *
 * Returns the root node of the new subtree which is replacing the
 * old one. */

static AVLTreeNode *avl_tree_node_balance(AVLTree *tree, AVLTreeNode *node) {
    AVLTreeNode *left_subtree;
    AVLTreeNode *right_subtree;
    AVLTreeNode *child;
    int diff;

    left_subtree = node->children[AVL_TREE_NODE_LEFT];
    right_subtree = node->children[AVL_TREE_NODE_RIGHT];

    /* 检查孩子节点的高度。如果不平衡，则旋转 */

    diff = avl_tree_subtree_height(right_subtree)
         - avl_tree_subtree_height(left_subtree);

    if (diff >= 2) {

        /* 偏向右侧太多 */

        child = right_subtree;

        if (avl_tree_subtree_height(child->children[AVL_TREE_NODE_RIGHT])
          < avl_tree_subtree_height(child->children[AVL_TREE_NODE_LEFT])) {

            /* 如果右孩子偏向左侧，它需要首先右旋转(双旋转) */

            avl_tree_rotate(tree, right_subtree,
                            AVL_TREE_NODE_RIGHT);
        }

        /* 进行左旋转.  After this, the right child will
         * take the place of this node.  Update the node pointer. */

        node = avl_tree_rotate(tree, node, AVL_TREE_NODE_LEFT);

    } else if (diff <= -2) {

        /* 偏向左侧太多 */

        child = node->children[AVL_TREE_NODE_LEFT];

        if (avl_tree_subtree_height(child->children[AVL_TREE_NODE_LEFT])
          < avl_tree_subtree_height(child->children[AVL_TREE_NODE_RIGHT])) {

            /* 如果左孩子偏向右侧，它需要首先左旋转(双旋转) */

            avl_tree_rotate(tree, left_subtree,
                            AVL_TREE_NODE_LEFT);
        }

        /* 进行右旋转.  After this, the left child will
         * take the place of this node.  Update the node pointer. */

        node = avl_tree_rotate(tree, node, AVL_TREE_NODE_RIGHT);
    }

    /* 更新节点的高度 */

    avl_tree_update_height(node);

    return node;
}

/* 从给定的节点开始向根节点遍历，进行需要的旋转操作 */

static void avl_tree_balance_to_root(AVLTree *tree, AVLTreeNode *node) {
    AVLTreeNode *rover;

    rover = node;

    while (rover != NULL) {

        /* 使节点平衡 */

        rover = avl_tree_node_balance(tree, rover);

        /* 指向该节点的双亲节点 */

        rover = rover->parent;
    }
}

AVLTreeNode *avl_tree_insert(AVLTree *tree, AVLTreeKey key, AVLTreeValue value) {
    AVLTreeNode **rover;
    AVLTreeNode *new_node;
    AVLTreeNode *previous_node;

    /* 遍历二叉平衡树，一直到空指针处 */

    rover = &tree->root_node;
    previous_node = NULL;

    while (*rover != NULL) {
        previous_node = *rover;
        if (tree->compare_func(key, (*rover)->key) < 0) {
            rover = &((*rover)->children[AVL_TREE_NODE_LEFT]);
        } else {
            rover = &((*rover)->children[AVL_TREE_NODE_RIGHT]);
        }
    }

    /* 创建一个新节点，以遍历的路径上最后一个节点为双亲节点 */

    new_node = (AVLTreeNode *) malloc(sizeof(AVLTreeNode));

    if (new_node == NULL) {
        return NULL;
    }

    new_node->children[AVL_TREE_NODE_LEFT] = NULL;
    new_node->children[AVL_TREE_NODE_RIGHT] = NULL;
    new_node->parent = previous_node;
    new_node->key = key;
    new_node->value = value;
    new_node->height = 1;

    /* 在遍历到达的空指针处插入节点 */

    *rover = new_node;

    /* 使树重新平衡 */

    avl_tree_balance_to_root(tree, previous_node);

    /* 更新节点数 */

    ++tree->num_nodes;

    return new_node;
}

/* 找到与给定节点关键字最接近的节点，将其从树中断开
 * The node returned is unlinked from the tree.
 * Returns NULL if the node has no children. */

static AVLTreeNode *avl_tree_node_get_replacement(AVLTree *tree,
                                                  AVLTreeNode *node) {
    AVLTreeNode *left_subtree;
    AVLTreeNode *right_subtree;
    AVLTreeNode *result;
    AVLTreeNode *child;
    int left_height, right_height;
    int side;

    left_subtree = node->children[AVL_TREE_NODE_LEFT];
    right_subtree = node->children[AVL_TREE_NODE_RIGHT];

    /* 无孩子节点? */

    if (left_subtree == NULL && right_subtree == NULL) {
        return NULL;
    }

    /* 从更高的子树中选择节点，以使树保持平衡 */

    left_height = avl_tree_subtree_height(left_subtree);
    right_height = avl_tree_subtree_height(right_subtree);

    if (left_height < right_height) {
        side = AVL_TREE_NODE_RIGHT;
    } else {
        side = AVL_TREE_NODE_LEFT;
    }

    /* 搜索关键字最接近的节点 */

    result = node->children[side];

    while (result->children[1-side] != NULL) {
        result = result->children[1-side];
    }

    /* 断开节点，如果它有孩子节点则取代其位置 */

    child = result->children[side];
    avl_tree_node_replace(tree, result, child);

    /* Update the subtree height for the result node's old parent. */

    avl_tree_update_height(result->parent);

    return result;
}

/* 删除一个节点 */

void avl_tree_remove_node(AVLTree *tree, AVLTreeNode *node) {
    AVLTreeNode *swap_node;
    AVLTreeNode *balance_startpoint;
    int i;

    /* 待删除的节点需要用关键字与其最接近的节点取代其位置，找到交换的节点 */

    swap_node = avl_tree_node_get_replacement(tree, node);

    if (swap_node == NULL) {

        /* 这是一个叶子节点，可以直接删除 */

        /* 将节点与其双亲断开 */

        avl_tree_node_replace(tree, node, NULL);

        /* 从原节点的双亲开始重新平衡 */

        balance_startpoint = node->parent;

    } else {
        /* 从交换的节点的原双亲开始重新平衡。当原双亲是待删除节点时，从交换的节点开始重新平衡 */

        if (swap_node->parent == node) {
            balance_startpoint = swap_node;
        } else {
            balance_startpoint = swap_node->parent;
        }

        /* 将与原节点相关的指针引用复制到交换的节点中 */

        for (i=0; i<2; ++i) {
            swap_node->children[i] = node->children[i];

            if (swap_node->children[i] != NULL) {
                swap_node->children[i]->parent = swap_node;
            }
        }

        swap_node->height = node->height;

        /* Link the parent's reference to this node */

        avl_tree_node_replace(tree, node, swap_node);
    }

    /* 销毁节点 */

    free(node);

    /* 更新节点数 */

    --tree->num_nodes;

    /* 使树重新平衡 */

    avl_tree_balance_to_root(tree, balance_startpoint);
}

/* 给定关键字，删除一个节点 */

int avl_tree_remove(AVLTree *tree, AVLTreeKey key) {
    AVLTreeNode *node;

    /* 找到待删除的节点 */

    node = avl_tree_lookup_node(tree, key);

    if (node == NULL) {
        /* 未找到 */

        return 0;
    }

    /* 删除节点 */

    avl_tree_remove_node(tree, node);

    return 1;
}

AVLTreeNode *avl_tree_lookup_node(AVLTree *tree, AVLTreeKey key) {
    AVLTreeNode *node;
    int diff;

    /* 在树中查找具有指定关键字的节点 */

    node = tree->root_node;

    while (node != NULL) {

        diff = tree->compare_func(key, node->key);

        if (diff == 0) {

            /* 关键字相同则返回该节点 */

            return node;

        } else if (diff < 0) {
            node = node->children[AVL_TREE_NODE_LEFT];
        } else {
            node = node->children[AVL_TREE_NODE_RIGHT];
        }
    }

    /* 未找到 */

    return NULL;
}

AVLTreeValue avl_tree_lookup(AVLTree *tree, AVLTreeKey key) {
    AVLTreeNode *node;

    /* 寻找节点 */

    node = avl_tree_lookup_node(tree, key);

    if (node == NULL) {
        return AVL_TREE_NULL;
    } else {
        return node->value;
    }
}

AVLTreeNode *avl_tree_root_node(AVLTree *tree) {
    return tree->root_node;
}

AVLTreeKey avl_tree_node_key(AVLTreeNode *node) {
    return node->key;
}

AVLTreeValue avl_tree_node_value(AVLTreeNode *node) {
    return node->value;
}

AVLTreeNode *avl_tree_node_child(AVLTreeNode *node, AVLTreeNodeSide side) {
    if (side == AVL_TREE_NODE_LEFT || side == AVL_TREE_NODE_RIGHT) {
        return node->children[side];
    } else {
        return NULL;
    }
}

AVLTreeNode *avl_tree_node_parent(AVLTreeNode *node) {
    return node->parent;
}

unsigned int avl_tree_num_entries(AVLTree *tree) {
    return tree->num_nodes;
}

static void avl_tree_to_array_add_subtree(AVLTreeNode *subtree,
                                         AVLTreeValue *array,
                                         int *index) {
    if (subtree == NULL) {
        return;
    }

    /* 先添加左子树的关键字 */

    avl_tree_to_array_add_subtree(subtree->children[AVL_TREE_NODE_LEFT],
                                  array, index);

    /* 在数组中添加该节点的关键字 */

    array[*index] = subtree->key;
    ++*index;

    /* 最后添加右子树的关键字 */

    avl_tree_to_array_add_subtree(subtree->children[AVL_TREE_NODE_RIGHT],
                                  array, index);
}

AVLTreeValue * avl_tree_to_array(AVLTree *tree) {
    AVLTreeValue *array;
    int index;

    /* 申请数组空间 */

    array = malloc(sizeof(AVLTreeValue) * tree->num_nodes);

    if (array == NULL) {
        return NULL;
    }

    index = 0;

    /* 添加所有的关键字 */

    avl_tree_to_array_add_subtree(tree->root_node, array, &index);

    return array;
}

