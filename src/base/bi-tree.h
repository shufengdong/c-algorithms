#ifndef ALGORITHM_BITREE_H
#define ALGORITHM_BITREE_H

#ifdef __cplusplus
extern "C" {
#endif

/* 二叉树的左、右孩子标记 */
typedef enum {
    BI_TREE_NODE_LEFT = 0,
    BI_TREE_NODE_RIGHT = 1
} BiTreeNodeSide;

/**
 * 二叉树结构
 *
 * @see bi_tree_new
 */
typedef struct _BiTree BiTree;

/**
 * 二叉树节点
 */
typedef struct _BiTreeNode BiTreeNode;

/**
 * 二叉树中存储的数据
 */
typedef void *BiTreeValue;

struct _BiTreeNode {
    BiTreeNode *children[2];
    BiTreeNode *parent;
    BiTreeValue value;
};

struct _BiTree {
    /* 根节点 */
    BiTreeNode *rootNode;
    /* 树中的节点数 */
    unsigned int nodeNum;

    /**
    * 销毁一棵二叉树
    * @param tree  The tree to destroy.
    */

    void (*free_tree)(BiTree *_this);

    /**
     * 给定双亲节点和左右的位置，插入一个节点
     *
     * @param tree            The tree.
     * @param value           The value to insert.
     * @param parent          The parent where to insert.
     * @param side            Insert to left or right.
     * @return                The newly created tree node containing the
     *                        key and value, or NULL if it was not possible
     *                        to allocate the new memory.
     */

    BiTreeNode * (*insert)(BiTree *_this, BiTreeValue value, BiTreeNode *parent, BiTreeNodeSide side);

    /**
     * 删除一个节点
     *
     * @param tree            The tree.
     * @param node            The node to remove
     */

    void (*remove)(BiTree *_this, BiTreeNode *node);


    /**
     * 查找二叉树的根节点
     *
     * @param tree            The tree.
     * @return                The root node of the tree, or NULL if the tree is
     *                        empty.
     */

    BiTreeNode *(*root_node)(BiTree *_this);


    /**
     * 获取给定节点中的数据
     *
     * @param node            The tree node.
     * @return                The value at the given node.
     */

    BiTreeValue (*node_value)(BiTreeNode *node);

    /**
     * 查找给定节点的孩子节点
     *
     * @param node            The tree node.
     * @param side            Which child node to get (left or right)
     * @return                The child of the tree node, or NULL if the
     *                        node has no child on the given side.
     */

    BiTreeNode *(*node_child)(BiTreeNode *node, BiTreeNodeSide side);

    /**
     * 查找给定节点的双亲节点
     *
     * @param node            The tree node.
     * @return                The parent node of the tree node, or NULL if
     *                        this is the root node.
     */

    BiTreeNode *(*node_parent)(BiTreeNode *node);
};


/**
 * A null @ref BiTreeValue.
 */
/* 二叉树的空指针数据 */
#define BI_TREE_NULL ((void *) 0)


/**
 * 建立一棵新的二叉树
 *
 * @return                A new binary tree, or NULL if it was not possible
 *                        to allocate the memory.
 */

BiTree * bi_tree_new();

/**
 * 初始化一颗二叉树
 *
 * @param tree  需要初始化的树.
 */
void bi_tree_initial(BiTree *tree);

/**
 * 删除一颗子树
 *
 * @param node 子树的根节点
 */
void bi_tree_free_subtree(BiTreeNode *node);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ALGORITHM_BITREE_H */