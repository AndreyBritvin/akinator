#include "akinator.h"
#include <assert.h>
#include "utils.h"
#include <stdio.h>
#include "my_tree.h"

// func to read from file  to buffer
__off_t get_file_len(const char *filename)
{
    assert(filename != NULL);

    struct stat st = {};
    stat(filename, &st);

    return st.st_size;
}

err_code_t fill_buffer(char **buffer_to_fill, const char* filename)
{
    size_t filesize = get_file_len(filename);
    char *temp_buf = (char *) calloc(filesize + 1, sizeof(char));
    if (temp_buf == NULL)
    {
        return ERROR_CALLOC_IS_NULL;
    }
    *buffer_to_fill = temp_buf;

    FILE* SAFE_OPEN_FILE(input_file, filename, "r");

    fread(temp_buf, 1, filesize, input_file);
    // printf("First symbol %c\n", temp_buf[0]);
    // printf("Readed value = %s\n", temp_buf);
    fclose(input_file);

    return OK;
}

my_tree_t make_tree(char *buffer)
{
    assert(buffer);

    INIT_TREE(tree_to_fill);

    free(tree_to_fill.root);
    tree_to_fill.root = fill_node(buffer, 0, &tree_to_fill, NULL);
    TREE_DUMP(&tree_to_fill, tree_to_fill.root, "I am gROOT");

    return tree_to_fill;
}

node_t* fill_node(char * buffer, size_t position, my_tree_t* tree, node_t* parent)
{
    assert(buffer);

    // char *expression = (char *) calloc(128, sizeof(char));
    SAFE_CALLOC(expression, char, MAX_STRING_SIZE);

    sscanf(buffer + position, "\"%[^\"]\"", expression);

    node_t* node_to_return = new_node(tree, expression, NULL, NULL);
    node_to_return->parent = parent;

    size_t i = position;
    while (buffer[i] != '}')
    {
        printf("Current char is %c position %zd\n", buffer[i], i);
        if (buffer[i] == '{')
        {
            node_to_return->right = fill_node(buffer, i + 1, tree, node_to_return);
        }
        i++;
    }

    return node_to_return;
}

// func to parse buffer to tree (recursively)
// func to make game - yes/no
// func to add new node
// func to overwrite new game
// func to make definitions (where store path? using parent field?)
// func to make comparison


