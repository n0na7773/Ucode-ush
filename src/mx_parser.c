#include "ush.h"

void func_push(t_ast **res, char *arg, int type, t_shell *shell) {
    t_export *q;
    char *tmp = mx_strdup(arg);
    char *args0 = mx_strtok(tmp, MX_USH_TOK_DELIM);
    for (q = shell->aliases; q; q = q->next) {
        if (mx_strcmp(args0, q->name) == 0 && !mx_strstr(q->value, args0)) {
            mx_strdel(&tmp);
            *res = mx_ush_parsed_line(*res, q->value, shell, type);
            return;
        }
    }
    for (q = shell->functions; q; q = q->next) {
        if (mx_strcmp(args0, q->name) == 0 && !mx_strstr(q->value, args0)) {
            mx_strdel(&tmp);
            *res = mx_ush_parsed_line(*res, q->value, shell, type);
            return;
        }
    }
    mx_strdel(&tmp);
    mx_ast_push_back(res, arg, type);
}

void recurcion_func_alias(t_ast **res, int old) {
    if (old) {
        t_ast *q = *res;
        for (;q->next;)
            q = q->next;
        q->type = old;
    }
}

bool isempty(char *s, char *delim) {
    if (!s || !delim)
        return true;
    for (int i = 0; s[i]; i++) {
        if (!mx_isdelim(s[i], delim))
            return false;
    }
    return true;
}

t_ast *mx_ush_parsed_line(t_ast *res, char *line1, t_shell *shell, int old) {
    char *line;
    int i = 0;
    char *tmp = NULL;
    int type = 0;

    if (mx_check_parce_errors(line1))
        return NULL;
    line = mx_strdup(line1);
    for (;line[i];) {
        if ((tmp = mx_get_token_and_delim(&line[i], &i, &type))) {
            if (!isempty(tmp, MX_USH_TOK_DELIM))
                func_push(&res, tmp, type, shell);
            else if (type != SEP)
                return mx_parse_error_ush(type, res, line);
            free(tmp);
        }
    }
    mx_strdel(&line);
    recurcion_func_alias(&res, old);
    return res;
}

t_ast **mx_ast_creation(char *line, t_shell *shell) {
    t_ast **ast = NULL;
    t_ast *parsed_line = NULL;

    if (!(parsed_line = mx_ush_parsed_line(parsed_line, line, shell, 0))) return NULL;

    if (!(ast = mx_ast_parse(parsed_line)) || !(*ast)) {
        mx_ast_clear_list(&parsed_line);
        return NULL;
    }

    mx_ast_clear_list(&parsed_line);
    return ast;
}

char *mx_ush_read_line(t_shell *shell) {
    size_t bufsize = 0;
    char *res = NULL;
    char *line = mx_strnew(1);

    if (getline(&line, &bufsize, stdin) < 0 && !isatty(0)) {
        shell->exit_code = 0;
        mx_clear_all(shell);
        exit(0);
    }
    if(line[0] != '\0'){
        res = mx_strdup(line);
        mx_strdel(&line);
    }
    return res;
}

static char **create_tokens(char **tokens_arr, int *bufsize_arr) {
    if (tokens_arr == NULL) {
        tokens_arr = malloc((*bufsize_arr) * sizeof(char*));

        if (!tokens_arr) {
            mx_printerr_red("ush: allocation error\n");
            return NULL;
        }
    }
    else {
        (*bufsize_arr) += 64;
        tokens_arr = realloc(tokens_arr, (*bufsize_arr) * sizeof(char*));

        if (!tokens_arr) {
            mx_printerr_red("ush: allocation error\n");
            return NULL;
        }
    }
    return tokens_arr;
}

char **mx_parce_tokens(char *str) {
    int bufsize = 64;
    int position = 0;
    char **tokens_arr = NULL;
    
    tokens_arr = create_tokens(tokens_arr, &bufsize);
    char *token_arr = mx_strtok(str, MX_USH_TOK_DELIM);

    while (token_arr != NULL) {
        tokens_arr[position] = token_arr;
        position++;
        if (position >= bufsize) {
            tokens_arr = create_tokens(tokens_arr, &bufsize);
        }
        token_arr = mx_strtok(NULL, MX_USH_TOK_DELIM);
    }
    tokens_arr[position] = NULL;
    return tokens_arr;
}
