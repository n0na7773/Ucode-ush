#include "ush.h"

int main(int argc, char **argv) {
    t_shell *shell = mx_init_shell(argc, argv);
    mx_ush_loop(shell);
    
    return MX_EXIT_SUCCESS;
}
