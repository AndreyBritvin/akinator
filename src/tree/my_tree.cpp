#include "my_tree.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "my_log.h"

static node_t* add_node_by_root(my_tree_t* tree, node_t* curr_node, tree_val_t data_to_add);

node_t* new_node(my_tree_t* tree, tree_val_t data, node_t* left_node, node_t* right_node)
{
    assert(tree);

    node_t* node = (node_t*) calloc(1, sizeof(node_t));

    node->data  =       data;
    node->left  =  left_node;
    node->right = right_node;

    return node;
}

err_code_t tree_ctor(my_tree_t* tree)
{
    tree->size = 1;
    tree->root = new_node(tree, 0, NULL, NULL);

    return OK;
}

err_code_t tree_dtor(my_tree_t* tree)
{
    CHECK_TREE(tree);
    node_dtor(tree->root);

    return OK;
}

err_code_t node_dtor(node_t* node)
{
    assert(node);

    if (node->left  != NULL) node_dtor(node->left);
    if (node->right != NULL) node_dtor(node->right);

    free(node->data);
    free(node);

    return OK;
}

err_code_t add_node(my_tree_t *tree, tree_val_t data_to_add)
{
    CHECK_TREE(tree);

    node_t* appended_node = add_node_by_root(tree, tree->root, data_to_add);
    tree->size += 1;

    TREE_DUMP(tree, appended_node, "Added this node");

    return OK;
}

static node_t* add_node_by_root(my_tree_t* tree, node_t* curr_node, tree_val_t data_to_add)
{
    if (curr_node == NULL) return new_node(tree, data_to_add, NULL, NULL);

    TREE_DUMP(tree, curr_node,
              "Comparing node.data = %d with data_to_add = %d", curr_node->data, data_to_add);

    node_t*node_to_return = NULL;
    if (curr_node->data < data_to_add)
    {
        node_to_return = add_node_by_root(tree, curr_node->right, data_to_add);
        if (curr_node->right == NULL)
        {
            curr_node->right = node_to_return;
            node_to_return->parent = curr_node;
        }
    }
    else
    {
        node_to_return = add_node_by_root(tree, curr_node->left, data_to_add);
        if (curr_node->left == NULL)
        {
            curr_node->left = node_to_return;
            node_to_return->parent = curr_node;
        }
    }

    return node_to_return;
}

err_code_t print_tree(my_tree_t* tree)
{
    CHECK_TREE(tree);

    print_node(tree->root);

    return OK;
}

err_code_t print_node(node_t* tree)
{
    if (tree == NULL) return ERROR_TREE_IS_NULL;

    printf("(");
    if (tree->left  != NULL) print_node(tree->left);
    printf("%d", tree->data);
    if (tree->right != NULL) print_node(tree->right);
    printf(")");

    return OK;
}

err_code_t verificator(my_tree_t* tree, node_t* node, size_t recurs_level)
{
    assert(tree);

    size_t index_to_append = 0;
    printf("Tree size is %zd, curr recurs is %zd\n", tree->size, recurs_level);
    if (recurs_level > tree->size)
    {
        return ERROR_TREE_LOOPED;
    }

    if (node->left  != NULL && node->left ->parent != node) return ERROR_PARENT_DONT_MATCH;
    if (node->right != NULL && node->right->parent != node) return ERROR_PARENT_DONT_MATCH;

    err_code_t err_code = OK;
    if (node->left  != NULL)                   err_code = verificator(tree, node->left,  recurs_level + 1);
    if (node->right != NULL && err_code == OK) err_code = verificator(tree, node->right, recurs_level + 1);

    return err_code;
}
