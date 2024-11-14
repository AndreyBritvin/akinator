#include "akinator.h"
#include <assert.h>
#include "utils.h"
#include <stdio.h>
#include "my_tree.h"
#include "my_stack.h"
#include <string.h>

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
    char* expression = (char *) calloc(MAX_STRING_SIZE, sizeof(char));
    if (expression == NULL)
    {
        fprintf(stderr, "Ended memory in fill_node in calloc\n");
        return NULL;
    }

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
    node_t* curr_node = tree->root;
    char user_input = 0;
    node_t* last_node = curr_node;
    while(curr_node != NULL)
    {
        last_node = curr_node;
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
    give_definition(tree, last_node);
    if (user_input == 'n')
    {
        add_new_object(tree, last_node);
    }

    return OK;
}

err_code_t give_definition(my_tree_t* tree, node_t* node_to_def)
{
    assert(tree);
    assert(node_to_def);

    my_stack_t path = {};
    INIT_STACK(path);
    node_t* pois_val = NULL;
    stack_ctor(&path, 0, sizeof(node_t*), print_longs, &pois_val);

    generate_path(tree, node_to_def, &path);
    print_path(node_to_def, &path);

    stack_dtor(&path);

    return OK;
}

err_code_t print_path(node_t* node_to_def, my_stack_t* stack)
{
    printf("%s - ", node_to_def->data);
    for (size_t i = stack->size; i > 1; i--)
    {
        node_t* def_node = NULL;
        stack_pop(stack, &def_node);
        printf("%s ", def_node->data);
        fflush(stdout);
    }
    printf("\n");

    return OK;
}

err_code_t generate_path(my_tree_t* tree, node_t* node_to_def, my_stack_t *stack)
{
    stack_push(stack, &node_to_def);

    if (node_to_def == tree->root)
    {
        return OK;
    }

    generate_path(tree, node_to_def->parent, stack);

    return OK;
}

err_code_t get_line_from_stdin(char ** what_to_change)
{
    SAFE_CALLOC(new_object, char, MAX_STRING_SIZE);
    fgets(new_object, MAX_STRING_SIZE, stdin);
    *strchr(new_object, '\n') = '\0';
    *what_to_change = new_object;

    return OK;
}

err_code_t add_new_object(my_tree_t* tree, node_t* which_to_swap)
{
    assert(tree);

    printf("Какой объект вы загадали?\n");

    char* new_object = NULL;
    get_line_from_stdin(&new_object);

    printf("Чем %s отличается от %s?\n", new_object, which_to_swap->data);

    char* new_comparison = NULL;
    get_line_from_stdin(&new_comparison);

    node_t* no_node  = new_node(tree, which_to_swap->data, NULL, NULL);
    node_t* yes_node = new_node(tree, new_object, NULL, NULL);

    which_to_swap->data  = new_comparison;
    which_to_swap->right = yes_node;
    which_to_swap->left  = no_node;
    yes_node->parent = no_node->parent = which_to_swap;

    TREE_DUMP(tree, which_to_swap, "added to this node two daughters");

    return OK;
}

err_code_t show_menu(my_tree_t* tree)
{
    char answer = 0;
    while (answer != MODE_END)
    {
        printf("What would you want to do?:\n"
               "%d - guess character, %d - overwrite base, %d - exit\n",
                MODE_GAME, MODE_REWRITE, MODE_END);
        scanf("%c", &answer);
        answer -= '0';
        free_input_buffer();
        if (answer == MODE_GAME)
        {
            play_game(tree);
        }
        else if (answer == MODE_REWRITE)
        {
            overwrite_file(tree);
        }
        else if (answer == MODE_END)
        {
            printf("Thank you for game. See you later (^-^)\n");
            break;
        }
        else if (answer == MODE_COMPARISON)
        {
            printf("This function still not ready :(\n");
        }
        else
        {
            printf("wrong input '%c'\n", answer);
        }
    }

    return OK;
}

err_code_t free_input_buffer()
{
    char garbage = 0;
    while (scanf("%c", &garbage) != EOF && garbage != '\n') {}

    return OK;
}

err_code_t overwrite_file(my_tree_t* tree)
{
    char * filename_to_overwrite = NULL;

    printf("Write filename where to overwrite file, or use current path: <PATH>\n");
    get_line_from_stdin(&filename_to_overwrite);
    FILE * SAFE_OPEN_FILE(overwrite_file, filename_to_overwrite, "w");

    write_node(tree, tree->root, 0, overwrite_file);

    free(filename_to_overwrite);
    fclose(overwrite_file);

    return OK;
}

err_code_t write_node(my_tree_t* tree, node_t* node, size_t recurs_level, FILE* overwrite_file)
{
    print_n_spaces(recurs_level * 4, overwrite_file);
    fprintf(overwrite_file, "{\"%s\"", node->data);
    if (node->left != NULL)
    {
        fprintf(overwrite_file, "\n");
        write_node(tree, node->left, recurs_level + 1, overwrite_file);
    }
    if (node->right != NULL)
    {
        write_node(tree, node->right, recurs_level + 1, overwrite_file);
    }
    if (node->right == NULL && node->left == NULL)
    {
        fprintf(overwrite_file, "}\n");
    }
    else
    {
        print_n_spaces(recurs_level * 4, overwrite_file);
        fprintf(overwrite_file, "}\n");
    }

    return OK;
}

err_code_t print_n_spaces(size_t num, FILE* where)
{
    for (size_t i = 0; i < num; i++)
    {
        fprintf(where, " ");
    }

    return OK;
}

//+func to parse buffer to tree (recursively)
//+func to make game - yes/no
//+func to add new node
//+func to overwrite new game
//+func to make definitions (where store path? using parent field?)
// func to make comparison


