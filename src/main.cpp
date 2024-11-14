#include "akinator.h"
#include "my_tree.h"
#include "my_log.h"
#include <stdio.h>

int main()
{
    enable_logging("tree_dump/akinator.html");
    char *buffer = 0;
    fill_buffer(&buffer, "database/data.txt");
    printf("%s\n", buffer);

    my_tree_t akinator_tree = make_tree(buffer);

    play_game(&akinator_tree);

    tree_dtor(&akinator_tree); // TODO: move to end_game()

    disable_logging();

    return OK;
}
