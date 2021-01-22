#include "ush.h"

static void get_exit_code(t_process *proc, int *exit_index, int *flag, int *start){
    if (proc->argv[1][0] == '+' || proc->argv[1][0] == '-') {
        if (proc->argv[1][1] == '\0') (*flag)++;
        (*start)++;
    }
    for (int i = *start; i < mx_strlen(proc->argv[1]); i++) {
        if (!mx_isdigit(proc->argv[1][i])) (*flag)++;
    }
    if (!*flag) {
        *exit_index = atoi(proc->argv[1]);
    } else {
        mx_printerr("ush: exit: ");
        mx_printerr(proc->argv[1]);
        mx_printerr(": numeric argument required\n");
        *exit_index = 255;
    }
}

static void real_exit(t_shell *shell, t_process *proc) {
    int exit_index = shell->exit_code;
    int flag = 0,  start = 0;

    if (proc->argv[1] != NULL) {
        get_exit_code(proc, &exit_index, &flag, &start);
    }
    mx_clear_all(shell);
    exit(exit_index);
}

int mx_exit(t_shell *shell, t_process *proc) {
    if (proc->argv[1] != NULL && proc->argv[2] != NULL) {
        mx_printerr("ush: exit: too many arguments\n");
    } else if (shell->jobs_stack->top && shell->exit_flag == 0) {
        mx_printerr("ush: you have suspended jobs.\n");
        shell->exit_flag = 1;
    } else {
        real_exit(shell, proc);
    }
    return 1;
}

int mx_false(t_shell *shell, t_process *p) {
    shell->exit_flag = 0;
    p->exit_code = 0;
    return 0;
}

int mx_true(t_shell *shell, t_process *p) {
    shell->exit_flag = 1;
    p->exit_code = 1;
    return 1;
}

void mx_clear_all(t_shell *shell) {
    mx_clear_export(shell->exported);
    mx_clear_export(shell->variables);
    mx_clear_export(shell->functions);
    mx_clear_export(shell->aliases);
    mx_del_strarr(&shell->history);
    free(shell->kernal);
    free(shell->pwd);
}

void mx_clear_export(t_export *list) {
    t_export *tmtmp_exp = list;
    t_export *tmp = NULL;

    if (!(list) || !list)
        return;
    while (tmtmp_exp) {
        if (tmtmp_exp->name) free(tmtmp_exp->name);
        if (tmtmp_exp->value) free(tmtmp_exp->value);

        tmp = tmtmp_exp->next;
        free(tmtmp_exp);
        tmtmp_exp = tmp;
    }
    list = NULL;
}

void mx_clear_list(t_list **list) {
    if (list == NULL || *list == NULL) return;
    
    t_list *first = *list;
    if (!first) return;

    while (first != NULL) {
        t_list *tmp = first->next;
        free(first);
        first = tmp;
    }
    *list = NULL;
}

void mx_ast_clear_list(t_ast **list) {
    if (!(*list) || !list) return;

    t_ast *tmtmp_exp = *list;
    t_ast *tmp = NULL;

    while (tmtmp_exp) {
        if (tmtmp_exp->token) free(tmtmp_exp->token);
        if (tmtmp_exp->args) mx_del_strarr(&tmtmp_exp->args);
        if (tmtmp_exp->left) mx_ast_clear_list(&tmtmp_exp->left);

        tmp = tmtmp_exp->next;
        free(tmtmp_exp);
        tmtmp_exp = tmp;
    }
    *list = NULL;
}

void mx_ast_clear_all(t_ast ***list) {
    t_ast **tmtmp_exp = *list;

    for (int i = 0; tmtmp_exp[i]; i++) mx_ast_clear_list(&tmtmp_exp[i]);

    free(tmtmp_exp);
    tmtmp_exp = NULL;
}

static void exit_ush(t_shell *shell) {
    printf("exit\n");
    mx_clear_all(shell);
    exit(EXIT_SUCCESS);
}
