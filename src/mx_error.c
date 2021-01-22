#include "ush.h"

static char check_quote_auditor(char *str, int *i) {
    int j = *i;
    char tmp;

    if (str[j] == '(' && str[j - 1] && str[j - 1] == '$')
        tmp = ')';
    else if (str[j] == '{' && str[j - 3] && !mx_strncmp(&str[j - 3], "() ", 3))
        tmp = '}';
    else if (str[j] == '(' || str[j] == ')' || str[j] == '{')
        return 0;
    else
        tmp = str[j];
    j++;
    while (str[j] && str[j] != tmp)
        (str[j] == '\\') ? (j += 2) : (j++);
    *i = j;

    if(tmp == ')') tmp = '(';

    return tmp;
}

static bool check_quote(char *str) {
    char tmp;

    for (int i = 0; str[i]; i++) {
        if (str[i] == '\\'){
            i++;
        } else if (mx_isdelim(str[i], "`\"(){")) {
            tmp = check_quote_auditor(str, &i);
            if (tmp && !str[i]) return mx_unmached_error(tmp);
        } else if (str[i] == '\'') {
            i++;
            while (str[i] && str[i] != '\'') i++;
            if (!str[i]) return mx_unmached_error('\'');
        }
    }
    return false;
}

static bool check_parse_auditor(char *line, int indx) {
    int i2, i3;

    i2 = mx_get_char_index_quote(&line[indx + 1], MX_PARSE_DELIM, MX_QUOTE);
    if (i2 == 0) {
        if (line[indx] != line[indx + 1] || line[indx + 1] == ';'){
            return mx_parse_error(&line[indx + 1], 1);
        } else if (line[indx + 2]) {
            i3 = mx_get_char_index_quote(&line[indx + 2],
                                         MX_PARSE_DELIM, MX_QUOTE);
            if (i3 == 0)
                return mx_parse_error(&line[indx + 2], 1);
        }
    }
    return false;
}

static bool check_parse(char *line) {
    int i = 0;

    while (line) {
        if ((i = mx_get_char_index_quote(line,
            MX_PARSE_DELIM, MX_QUOTE)) >= 0) {
            if ((line[i + 1] == '\0' && line[i] != ';' && line[i] != '&') || mx_strcmp(&line[i], "&&") == 0)
                return mx_parse_error("\\n", 2);
            if (line[i + 1])
                if (check_parse_auditor(line, i)) return true;
            line += i + 1;
        } else {
            break;
        }
    }
    return false;
}

bool mx_check_parce_errors(char *line) {
    if (!line || check_quote(line) || check_parse(line)) return true;

    if (line[0] && mx_isdelim(line[0], "|&><")) {
        if (line[1] && mx_isdelim(line[1], "|&><"))
            return mx_parse_error(&line[0], 2);
        else
            return mx_parse_error(&line[0], 1);
    }
    return false;
}

void mx_printerr_red(char *c) {
    mx_printerr(MX_RED);
    mx_printerr(c);
    mx_printerr(MX_RESET);
}

void mx_sig_h(int signal) {
    if (signal == SIGPIPE) {
        mx_printerr("err write PIPE!!!!\n");
    }
}

char *usage_func_error(void) {
    mx_printerr("ush: function usage: func() { ...; }\n");
    return NULL;
}

void mx_print_error(char *command, char *error) {
    mx_printerr("ush: ");
    if (error) {
        mx_printerr(command);
        mx_printerr(error);
    }
    else
        perror(command);
}

char *error_getter(char **name_arr, char *command, int *status) {
    *status = 127;
    char *error = NULL;

    if (strstr(command, "/")) {
        struct stat buff;
        *name_arr = command;

        if (lstat(*name_arr, &buff) < 0) {
            error = NULL;
        }
        else {
            if (mx_get_type(buff) == 'd') {
                error = strdup("is a directory: ");
                *status = 126;
            }
        }
    }
    else {
        error = strdup("command not found: ");
    }
    
    return error;
}

char *error_getter_bin(char **name, char *command, int *status) {
    char *error = NULL;

    *status = 127;
    if (strstr(command, "/")) {
        struct stat buff;
        *name = command;
        if (lstat(*name, &buff) < 0) {
            error = NULL;
        }
        else {
            if (mx_get_type(buff) == 'd') {
                error = strdup(": is a directory\n");
                *status = 126;
            }
        }
    }
    return error;
}

void print_error_env(char *command, char *error) {
    mx_printerr("env: ");

    if (error) {
        mx_printerr(command);
        mx_printerr(error);
        free(error);
    }
    else {
        perror(command);
    }
}
