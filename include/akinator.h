#ifndef AKINATOR_H_
#define AKINATOR_H_
#include "utils.h"
#include "my_tree.h"
#include <sys/stat.h>
#include "my_stack.h"

enum game_mods
{
    MODE_GAME                       = 0,
    MODE_SHOW_ALL_TEXT              = 1,
    MODE_SHOW_ALL_DEFINITIONS       = 2,
    MODE_DEFINITION                 = 3,
    MODE_COMPARISON                 = 4,
    MODE_REWRITE                    = 5,
    MODE_END                        = 6,
};

const int MAX_STRING_SIZE = 128;

err_code_t fill_buffer(char **buffer_to_fill, const char* filename);
my_tree_t make_tree(char *buffer);
node_t* fill_node(char * buffer, size_t* position, my_tree_t* tree, node_t* parent);
size_t get_file_len(const char *filename);
err_code_t play_game(my_tree_t* tree);
err_code_t show_menu(my_tree_t* tree);
err_code_t free_input_buffer();
err_code_t add_new_object(my_tree_t* tree);
err_code_t give_definition(my_tree_t* tree, node_t* node_to_def);
err_code_t print_path(node_t* node_to_def, my_stack_t* stack);
err_code_t generate_path(my_tree_t* tree, node_t* node_to_def, my_stack_t *stack);
err_code_t add_new_object(my_tree_t* tree, node_t* which_to_swap);
err_code_t overwrite_file(my_tree_t* tree);
err_code_t get_line_from_stdin(char ** what_to_change);
err_code_t write_node(my_tree_t* tree, node_t* node, size_t recurs_level, FILE* overwrite_file);
err_code_t print_n_spaces(size_t num, FILE* where);
node_t* find_node_by_text(my_tree_t* tree, node_t* node, char * str_to_find);
err_code_t print_all_text(my_tree_t* tree, node_t* node);
err_code_t print_all_definitions(my_tree_t* tree, node_t* node);
err_code_t compare_objects(my_tree_t* tree, node_t* node_to_cmp_1, node_t* node_to_cmp_2);
err_code_t print_comparison(node_t* node_1, node_t* node_2, my_stack_t* path_1, my_stack_t* path_2);

#endif // AKINATOR_H_
