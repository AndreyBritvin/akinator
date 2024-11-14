#include "akinator.h"
#include <assert.h>
#include "utils.h"
#include <stdio.h>
#include "my_tree.h"

// func to read from file  to buffer
size_t get_file_len(const char *filename)
{
    assert(filename != NULL);

    struct stat st = {};
    stat(filename, &st);

    return (size_t) st.st_size;
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

    size_t position = 0;
    tree_to_fill.root = fill_node(buffer, &position, &tree_to_fill, NULL);
    TREE_DUMP(&tree_to_fill, tree_to_fill.root, "I am gROOT");

    return tree_to_fill;
}

node_t* fill_node(char * buffer, size_t* position, my_tree_t* tree, node_t* parent)
{// TODO refactor
    assert(buffer);
    assert(position);

    (*position)++;
    // char *expression = (char *) calloc(128, sizeof(char));
    SAFE_CALLOC(expression, char, MAX_STRING_SIZE);

    sscanf(buffer + *position, "\"%[^\"]\"", expression);

    tree->size++;
    node_t* node_to_return = new_node(tree, expression, NULL, NULL);
    node_to_return->parent = parent;
    if (*position == 1)
    {
        tree->root = node_to_return;
    }

    bool is_left = true;

    while (buffer[*position] != '}')
    {
        // TREE_DUMP(tree, node_to_return, "Working with this\n Input text = %s", expression);
        printf("Current char is %c position %zu\n", buffer[*position], *position);
        if (buffer[*position] == '{' && is_left)
        {
            is_left = false;
            node_to_return->left = fill_node(buffer, position, tree, node_to_return);
        }

        else if (buffer[*position] == '{' && !is_left)
        {
            node_to_return->right = fill_node(buffer, position, tree, node_to_return);
        }

        (*position)++;
    }

    return node_to_return;
}

err_code_t play_game(my_tree_t* tree)
{
    char user_input = 0;

    node_t* curr_node = tree->root;
    do
    {
        while(curr_node != NULL)
        {
            printf("Ваш объект %s? Y/n\n", curr_node->data);
            scanf("%c", &user_input);
            free_input_buffer();
            if (user_input == 'Y' || user_input == 'y')
            {
                curr_node = curr_node->right;
            }
            else if (user_input == 'n')
            {
                curr_node = curr_node->left;
            }
            else
            {
                printf("Input '%c' is not valid. Input Y/n\n", user_input);
            }
        }

    }while (show_menu() == MODE_GAME);

    return OK;
}

err_code_t show_menu()
{
    char answer = 0;
    printf("Do you want to guess new game? Y/n\n");
    scanf("%c", &answer);
    if (answer == 'Y')
    {
        return MODE_GAME;
    }
    printf("Do you want to compare two objects? Y/n\n");
    scanf("%c", &answer);
    if (answer == 'Y')
    {
        return MODE_COMPARISON;
    }
    printf("Do you want to rewrite file? Y/n\n");
    scanf("%c", &answer);
    if (answer == 'Y')
    {
        return MODE_REWRITE;
    }

    return MODE_END;
}

err_code_t free_input_buffer()
{
    char garbage = 0;
    while (scanf("%c", &garbage) != EOF && garbage != '\n')
    {

    }

    return OK;
}

//+func to parse buffer to tree (recursively)
// func to make game - yes/no
// func to add new node
// func to overwrite new game
// func to make definitions (where store path? using parent field?)
// func to make comparison


