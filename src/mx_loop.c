#include "ush.h"

int get_job_type(t_ast **ast_arr, int i) {
    t_ast *ast_temp = NULL;

    if (i != 0) {
        ast_temp = ast_arr[i - 1];
        while (ast_temp->next) {
            ast_temp = ast_temp->next;
        }
        if (ast_temp->type == AND || ast_temp->type == OR) {
            return ast_temp->type;
        }
    }

    return 0;
}

void launch_blow_job(t_shell *shell, t_ast **ast_arr) {
    t_job *new_job = NULL;

    for (int i = 0; ast_arr[i]; i++) {
        if ((new_job = mx_create_job(shell, ast_arr[i]))) {
            new_job->job_type = get_job_type(ast_arr, i);
            mx_launch_job(shell, new_job);
        }
    }

    mx_ast_clear_all(&ast_arr);
}

void mx_ush_loop(t_shell *shell) {
    char *str = NULL;
    t_ast **ast_arr = NULL;

    getenv("HOME") ? shell->git = mx_get_git_info() : 0;
    
    while (1) {
        isatty(0) ? (str = mx_get_line(shell)) : (str = mx_ush_read_line(shell));
        if (str[0] == '\0') {
            free(str);
            mx_check_jobs(shell);
            continue;
        }
        else if ((ast_arr = mx_ast_creation(str, shell))) {
            launch_blow_job(shell, ast_arr);
        }
        mx_strdel(&str);
    }
}
