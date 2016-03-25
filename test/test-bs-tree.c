#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "alloc-testing.h"
#include "framework.h"

#include "bs-tree.h"
#include "compare-int.h"

/* Tree print function - useful for debugging. */

static void print_tree(BsTreeNode *node, int depth) {
    int *value;
    int i;

    if (node == NULL) {
        return;
    }

    print_tree(((BiTreeNode *)node)->children[BI_TREE_NODE_LEFT], depth + 1);

    for (i = 0; i < depth*6; ++i) {
        printf(" ");
    }

    value = node->key;
    printf("%i\n", *value);

    print_tree(((BiTreeNode *)node)->children[BI_TREE_NODE_RIGHT], depth + 1);
}

void test_bs_tree_new() {
    int keys[] = {63, 90, 70, 55, 67, 42, 98, 83, 10, 45, 58};
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    int i, n = 11;

    BsTree *tree = bs_tree_new((BsTreeCompareFunc) int_compare);

    assert(tree != NULL);
    assert(((BiTree *)tree)->root_node(tree) == NULL);
    assert(((BiTree *)tree)->nodeNum == 0);
    /*插入一些数据*/
    for(i = 0; i < n; i++)
        tree->insert_key_value(tree, &keys[i], &values[i]);
    /*开始验证*/
    for(i = 0; i < n; i++) {
        int v = *(int *)(tree->lookup_value(tree, &keys[i]));
        assert(v == values[i]);
    }
    print_tree(((BiTree *)tree)->root_node(tree), 0);
    ((BiTree *)tree)->free_tree(tree);
}

static UnitTestFunction tests[] = {
    test_bs_tree_new,
    NULL
};

int main(int argc, char *argv[]) {
    run_tests(tests);
    return 0;
}