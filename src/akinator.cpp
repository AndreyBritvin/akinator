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
    if (user_input == 'y')
    {
        give_definition(tree, last_node);
    }
    else
    {
        add_new_object(tree, last_node);
    }

    return OK;
}

err_code_t give_definition(my_tree_t* tree, node_t* node_to_def)
{
    assert(tree);

    if (node_to_def == NULL)
    {
        printf("Unknown name...\n");

        return OK;
    }

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
    printf("%s -", node_to_def->data);
    for (size_t i = stack->size; i > 1; i--)
    {
        node_t* def_node = NULL;
        stack_pop(stack, &def_node);
        insert_not(def_node, stack);
        printf(" %s", def_node->data);
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

err_code_t show_menu(my_tree_t* tree, const char* curr_filename)
{
    char answer = 0;
    while (answer != MODE_END)
    {
        printf("What would you want to do?:\n"
               "%d - guess character\n"
               "%d - print all objects\n"
               "%d - print all definitions\n"
               "%d - definition mode (using input, not finding mode)\n"
               "%d - comparison mode\n"
               "%d - overwrite base\n"
               "%d - exit\n",
                MODE_GAME, MODE_SHOW_ALL_TEXT, MODE_SHOW_ALL_DEFINITIONS,
                MODE_DEFINITION, MODE_COMPARISON, MODE_REWRITE, MODE_END);
        scanf("%c", &answer);
        answer -= '0';
        free_input_buffer();
        if (answer == MODE_GAME)
        {
            play_game(tree);
        }
        else if (answer == MODE_REWRITE)
        {
            overwrite_file(tree, curr_filename);
        }
        else if (answer == MODE_END)
        {
            printf("Thank you for game. See you later (^-^)\n");
            break;
        }
        else if (answer == MODE_COMPARISON)
        {
            char* what_to_cmp_1 = NULL;
            char* what_to_cmp_2 = NULL;

            printf("Write name of object 1 to compare:\n");
            get_line_from_stdin(&what_to_cmp_1);

            printf("Write name of object 2 to compare:\n");
            get_line_from_stdin(&what_to_cmp_2);

            compare_objects(tree, find_node_by_text(tree, tree->root, what_to_cmp_1),
                                  find_node_by_text(tree, tree->root, what_to_cmp_2));

            free(what_to_cmp_1);
            free(what_to_cmp_2);
        }
        else if (answer == MODE_DEFINITION)
        {
            printf("Write name of object to give definition:\n");
            char* what_to_def = NULL;
            get_line_from_stdin(&what_to_def);
            give_definition(tree, find_node_by_text(tree, tree->root, what_to_def));
            free(what_to_def);
        }
        else if (answer == MODE_SHOW_ALL_TEXT)
        {
            print_all_text(tree, tree->root);
        }
        else if (answer == MODE_SHOW_ALL_DEFINITIONS)
        {
            print_all_definitions(tree, tree->root);
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

err_code_t overwrite_file(my_tree_t* tree, const char* filename)
{
    assert(filename);

    char * filename_to_overwrite = NULL;

    printf("Write filename where to overwrite file, or use current path: <%s>\n", filename);
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

node_t* find_node_by_text(my_tree_t* tree, node_t* node, char * str_to_find)
{
    node_t* node_to_return = NULL;

    if (!strcmp(node->data, str_to_find))
    {
        TREE_DUMP(tree, node, "comparing this data with %s", str_to_find);
        node_to_return = node;
    }

    if (node->left  != NULL && node_to_return == NULL)
                               node_to_return = find_node_by_text(tree, node->left , str_to_find);
    if (node->right != NULL && node_to_return == NULL)
                               node_to_return = find_node_by_text(tree, node->right, str_to_find);

    return node_to_return;
}

err_code_t print_all_text(my_tree_t* tree, node_t* node)
{
    if (node->left == NULL && node->right == NULL) printf(" - %s\n", node->data);

    if (node->left  != NULL) print_all_text(tree, node->left);
    if (node->right != NULL) print_all_text(tree, node->right);

    return OK;
}

err_code_t print_all_definitions(my_tree_t* tree, node_t* node)
{
    if (node->left == NULL && node->right == NULL) give_definition(tree, node);

    if (node->left  != NULL) print_all_definitions(tree, node->left);
    if (node->right != NULL) print_all_definitions(tree, node->right);

    return OK;
}

err_code_t compare_objects(my_tree_t* tree, node_t* node_to_cmp_1, node_t* node_to_cmp_2)
{
    assert(tree);

    if (node_to_cmp_1 == NULL)
    {
        printf("Unknown name for first name...\n");

        return OK;
    }
    if (node_to_cmp_2 == NULL)
    {
        printf("Unknown name for second name...\n");

        return OK;
    }

    node_t* pois_val = NULL;

    my_stack_t path1 = {};
    INIT_STACK(path1);
    stack_ctor(&path1, 0, sizeof(node_t*), print_longs, &pois_val);

    my_stack_t path2 = {};
    INIT_STACK(path2);
    stack_ctor(&path2, 0, sizeof(node_t*), print_longs, &pois_val);


    generate_path(tree, node_to_cmp_1, &path1);
    generate_path(tree, node_to_cmp_2, &path2);
    print_comparison(node_to_cmp_1, node_to_cmp_2, &path1, &path2);

    stack_dtor(&path1);
    stack_dtor(&path2);

    return OK;
}

err_code_t print_comparison(node_t* node_1, node_t* node_2, my_stack_t* path_1, my_stack_t* path_2)
{
    printf("И %s, и %s -", node_1->data, node_2->data);
    for (size_t i = path_1->size, j = path_2->size; i > 1 && j > 1; i--, j--)
    {
        node_t* node_cmp_1 = NULL;
        node_t* node_cmp_2 = NULL;

        stack_pop(path_1, &node_cmp_1);
        stack_pop(path_2, &node_cmp_2);

        if (node_cmp_1 == node_cmp_2)
        {
            insert_not(node_cmp_1, path_1);
            printf(" %s", node_cmp_1->data);
        }
        else
        {
            printf(", но %s", node_1->data);
            for (; i > 1; i--)
            {
                insert_not(node_cmp_1, path_1);
                printf(" %s", node_cmp_1->data);
                stack_pop(path_1, &node_cmp_1);
            }
            printf(", а %s", node_2->data);
            for (; j > 1; j--)
            {
                insert_not(node_cmp_2, path_2);
                printf(" %s", node_cmp_2->data);
                stack_pop(path_2, &node_cmp_2);
            } // TODO: make negative expressions
        }
    }
    printf("\n");

    return OK;
}

err_code_t insert_not(node_t* curr_node, my_stack_t* stack)
{
    if (curr_node->left == NULL && curr_node->right == NULL)
    {
        return ERROR_CANT_GET_NEXT_NODE;
    }

    node_t* next_node = NULL;
    stack_look(stack, &next_node);

    if (next_node->parent->left == next_node)
    {
        printf(" not");
    }

    return OK;
}

err_code_t stack_look(my_stack_t* stack, void* where_to_look) // TODO: move to stack
{
    memcpy(where_to_look, (char *) stack->data + (stack->size - 1) * stack->elem_size, stack->elem_size);

    return OK;
}

//+func to parse buffer to tree (recursively)
//+func to make game - yes/no
//+func to add new node
//+func to overwrite new game
//+func to make definitions (where store path? using parent field?)
//+func to make comparison


