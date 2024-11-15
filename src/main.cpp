#include "akinator.h"
#include "my_tree.h"
#include "my_log.h"
#include <stdio.h>

int main(const int argc, const char** argv)
{
    if (argc == 1)
    {
        printf("Please enter <%s path_to_database> to start game\n", argv[0]);
        return OK;
    }
    enable_logging("tree_dump/akinator.html");
    char *buffer = 0;
    fill_buffer(&buffer, argv[1]);
    printf("%s\n", buffer);

    my_tree_t akinator_tree = make_tree(buffer);

    show_menu(&akinator_tree, argv[1]);

    tree_dtor(&akinator_tree); // TODO: move to end_game()

    disable_logging();

    return OK;
}
