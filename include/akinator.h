#ifndef AKINATOR_H_
#define AKINATOR_H_
#include "utils.h"
#include "my_tree.h"
#include <sys/stat.h>

enum game_mods
{
    MODE_GAME = 0,
    MODE_DEFINITION,
    MODE_COMPARISON,
    MODE_REWRITE,
    MODE_END,
};

const int MAX_STRING_SIZE = 128;

err_code_t fill_buffer(char **buffer_to_fill, const char* filename);
my_tree_t make_tree(char *buffer);
node_t* fill_node(char * buffer, size_t* position, my_tree_t* tree, node_t* parent);
size_t get_file_len(const char *filename);
err_code_t play_game(my_tree_t* tree);
err_code_t show_menu();
err_code_t free_input_buffer();

#endif // AKINATOR_H_
