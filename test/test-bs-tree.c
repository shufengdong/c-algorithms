#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "alloc-testing.h"
#include "framework.h"

#include "bs-tree.h"
#include "compare-int.h"

void test_bs_tree_new() {
    int keys[] = {63, 90, 70, 55, 67, 42, 98, 83, 10, 45, 58};
    int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    int i, n = 11;
    BsTree *tree;

    tree = bs_tree_new((BsTreeCompareFunc) int_compare);

    assert(tree != NULL);
    assert(((BiTree *)tree)->root_node(tree) == NULL);
    assert(((BiTree *)tree)->nodeNum == 0);

    for(i = 0; i < n; i++)
        tree->insert_key_value(tree, &keys[i], &values[i]);
    for(i = 0; i < n; i++) {
        int v = *(int *)(tree->lookup_value(tree, &keys[i]));
        assert(v == values[i]);
    }
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