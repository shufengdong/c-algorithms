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

    /* 遍历二叉查找树，一直到空指针处 */

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

    /* 创建一个新节点，以遍历的路径上最后一个节点为双亲节点 */

    newNode = (BsTreeNode *) malloc(sizeof(BsTreeNode));

    if (newNode == NULL) {
        return NULL;
    }

    ((BiTreeNode *)newNode)->children[BI_TREE_NODE_LEFT] = NULL;
    ((BiTreeNode *)newNode)->children[BI_TREE_NODE_RIGHT] = NULL;
    ((BiTreeNode *)newNode)->parent = previousNode;
    ((BiTreeNode *)newNode)->value = value;
    newNode->key = key;

    /* 在遍历到达的空指针处插入节点 */

    *rover = newNode;

    /* 更新节点数 */

    ++((BiTree *)tree)->nodeNum;

    return newNode;
}


static void bs_tree_remove_node(BsTree *_this, BsTreeNode * node) {
    bs_tree_remove_by_key(_this, node->key);
}

static int bs_tree_remove_by_key(BsTree *_this, BsTreeKey key) {
    BsTreeNode *node, *tmpCell;
    	node = bs_tree_lookup_node(_this, key);
    	int keyValue;
    	if(node != NULL) {
    		/* 两个孩子节点 */
    		if(node->base.children[BI_TREE_NODE_LEFT] != NULL &&
    				node->base.children[BI_TREE_NODE_RIGHT] != NULL) {
    			/* 用右子树的最小节点替换 */
    			tmpCell = bs_tree_find_min(node->base.children[BI_TREE_NODE_RIGHT]);
    			keyValue = *((int *)tmpCell->key);
    			bs_tree_remove_by_key (_this, tmpCell->key);
    			node->key = &keyValue;
    		}
    		else {	/* 有一个或无孩子节点 */
    			if(node->base.children[BI_TREE_NODE_LEFT] == NULL) {
    				/* 判断node是左孩子还是右孩子节点指针 */
    				if(int_compare(((BsTreeNode *)node->base.parent)->key, node->key))
    					node->base.parent->children[BI_TREE_NODE_LEFT] = node->base.children[BI_TREE_NODE_RIGHT];
    				else
    					node->base.parent->children[BI_TREE_NODE_RIGHT] = node->base.children[BI_TREE_NODE_RIGHT];
    			}
    			else if(node->base.children[BI_TREE_NODE_RIGHT] == NULL) {
    				if(int_compare(((BsTreeNode *)node->base.parent)->key, node->key))
    					node->base.parent->children[BI_TREE_NODE_LEFT] = node->base.children[BI_TREE_NODE_LEFT];
    				else
    					node->base.parent->children[BI_TREE_NODE_RIGHT] = node->base.children[BI_TREE_NODE_LEFT];
    			}
    			free(node);
    		}
    		return 1;
    	}
    return 0;
}

static BsTreeNode * bs_tree_lookup_node(BsTree *tree, BsTreeKey key) {
    BsTreeNode *node;
    int diff;

    /* 搜索二叉查找树并且寻找含有特定关键字的节点 */

    node = ((BiTree *)tree)->rootNode;

    while (node != NULL) {

        diff = tree->compareFunc(key, node->key);

        if (diff == 0) {

            /* 关键字相同，返回该节点 */

            return node;

        } else if (diff < 0) {
            node = ((BiTreeNode *)node)->children[BI_TREE_NODE_LEFT];
        } else {
            node = ((BiTreeNode *)node)->children[BI_TREE_NODE_RIGHT];
        }
    }

    /* 未找到 */

    return NULL;
}

BiTreeValue bs_tree_lookup(BsTree *tree, BsTreeKey key) {
    BsTreeNode *node;

    /* 寻找节点 */

    node = bs_tree_lookup_node(tree, key);

    if (node == NULL) {
        return BI_TREE_NULL;
    } else {
        return ((BiTreeNode *)node)->value;
    }
}


/* 建立一棵新的二叉查找树 */

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