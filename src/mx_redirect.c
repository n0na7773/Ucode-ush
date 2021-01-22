#include "ush.h"

static int count_sep_first_lwl(t_ast *q_ast) {
    int i = 1;

    for (; q_ast; q_ast = q_ast->next)
        if (MX_IS_SEP_FIRST_LWL(q_ast->type))
            i++;
    return i;
}

t_ast *push_redirections(t_ast **q_ast_arr, t_ast **ast) {
    int temp_type;
    t_ast *c = (*q_ast_arr)->next;

    temp_type = (*q_ast_arr)->type;
    for (; c && MX_IS_REDIRECTION(temp_type); c = c->next, (*q_ast_arr) = (*q_ast_arr)->next) {
        mx_ast_push_back_redirection(ast, c->token, temp_type);
        temp_type = c->type;
    }

    for (c = *ast; c->next; c = c->next);
    c->type = temp_type;

    return *q_ast_arr;
}

t_ast **mx_ast_parse(t_ast *parsed_line) {
    t_ast *q = parsed_line;
    int k = count_sep_first_lwl(q);
    int i = 0;
    t_ast **ast = (t_ast **)malloc((k + 1) * sizeof(t_ast *));

    ast[i] = NULL;
    for (; q; q = q->next) {
        mx_ast_push_back(&ast[i], q->token, q->type);
        if (MX_IS_REDIRECTION(q->type)) {
            q = push_redirections(&q, &ast[i]);
        }
        if (MX_IS_SEP_FIRST_LWL(q->type) || q->type == NUL) {
            ast[++i] = NULL;
        }
    }
    return ast;
}


t_ast *ast_create_node(char *arg, int type) {
    t_ast *ast_q;

    if (!arg) {
        return NULL;
    }

    ast_q = (t_ast *)malloc(sizeof(t_ast));
    if (!ast_q) {
        return NULL;
    }

    ast_q->args = NULL;
    ast_q->token = strdup(arg);
    ast_q->type = type;
    ast_q->next = NULL;
    ast_q->left = NULL;

    return ast_q;
}

void mx_ast_push_back(t_ast **ast_head, char *arg, int type) {
    t_ast *ast_q;
    t_ast *ast_p;

    if (!ast_head || !arg) {
        return;
    }

    ast_q = ast_create_node(arg, type);
    if (!ast_q) {
        return;
    }

    ast_p = *ast_head;
    if (*ast_head == NULL) {
        *ast_head = ast_q;
        return;
    }
    else {
        while (ast_p->next != NULL)
            ast_p = ast_p->next;
        ast_p->next = ast_q;
    }
}

void mx_ast_push_back_redirection(t_ast **ast_head, char *arg, int type) {
    t_ast *ast_p;

    if (!ast_head || !arg) {
        return;
    }

    ast_p = *ast_head;
    if (*ast_head == NULL) {
        mx_printerr("u$h: trying to connect redirection to empry process.\n");
        return;
    }
    else {
        while (ast_p->next != NULL) {
            ast_p = ast_p->next;
        }
        mx_ast_push_back(&ast_p->left, arg, type);
    }
}

t_redir *redir_create_node(char *path, int type) {
    t_redir *redir_q;

    if (!path) {
        return NULL;
    }

    redir_q = (t_redir *)malloc(sizeof(t_redir));
    if (!redir_q) {
        return NULL;
    }

    redir_q->input_path = NULL;
    redir_q->output_path = NULL;

    if (MX_IS_REDIR_INP(type)) {
        redir_q->input_path = mx_strdup(path);
    }
    else if (MX_IS_REDIR_OUTP(type)) {
        redir_q->output_path = mx_strdup(path);
    }
    else {
        return NULL;
    }

    redir_q->redir_delim = type;
    redir_q->next = NULL;
    
    return (redir_q);
}

void mx_redir_push_back(t_redir **redir_head, char *path, int type) {
    t_redir *redir_q;
    t_redir *redir_p;

    if (!redir_head || !path) {
        return;
    }

    redir_q = redir_create_node(path, type);
    if (!redir_q) {
        return;
    }

    redir_p = *redir_head;
    if (*redir_head == NULL) {
        *redir_head = redir_q;
        return;
    }
    else {
        while (redir_p->next != NULL) {
            redir_p = redir_p->next;
        }
        redir_p->next = redir_q;
    }
}

void mx_redir_clear_list(t_redir **list) {
    t_redir *redir_q = *list;
    t_redir *redir_temp = NULL;

    if (!(*list) || !list) {
        return;
    }

    while (redir_q) {
        mx_strdel(&redir_q->input_path);
        mx_strdel(&redir_q->output_path);
        redir_temp = redir_q->next;
        free(redir_q);
        redir_q = redir_temp;
    }

    *list = NULL;
}
