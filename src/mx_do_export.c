#include "ush.h"

int export_count_args(char **args, int option_num) {
    int args_num = 0;
    for (int i = option_num; args[i] != NULL; i++)args_num++;
    return args_num;
}

static void print_export(t_export *export) {
    t_export *head_exp = export;

    while (head_exp != NULL) {
        if (strncmp(head_exp->name,"BASH_FUNC_",10) != 0) {
            printf("export %s", head_exp->name);
            if (head_exp->value) printf("=\"%s\"", head_exp->value);
            printf("\n");
        }
        head_exp = head_exp->next;
    }
}

void mx_clear_data(char *name, char *value) {
    if(name) free(name);
    if(value) free(value);
}

int mx_export(t_shell *shell, t_process *proc) {
    int option_num = mx_count_options(proc->argv, "proc", "export", " [name[=value] ...] or export -proc");
    int args_num = export_count_args(proc->argv, option_num);
    int a = 0;
    int exit_index = 0;

    if (option_num <  0) return 1;
    if (args_num == 1){
        print_export(shell->exported);
    } else {
        a = option_num + 1;
        while (proc->argv[a] != NULL) {
            mx_export_or_error(proc->argv[a], shell->exported, shell->variables, &exit_index);
            a++;
        }
    }
    return exit_index;
}

static int check_identifier(char *arg) {
    int in_flag = 0;
    int len_name = mx_get_char_index(arg, '=');

    if (len_name == 0) {
        in_flag++;
        return in_flag;
    }
    for (int j = 0; j < len_name; j++) {
        if (!isalpha(arg[j]) && !isdigit(arg[j]) && arg[j] != '_') in_flag++;
    }
    return in_flag;
}

void print_export_error(char *arg, int *exit_index) {
    mx_printerr("ush: export: `");
    mx_printerr(arg);
    mx_printerr("': not a valid identifier\n");
    *exit_index = 1;
}

static void export_value(t_export *export, char *name, char *value) {
    int flag = 0;
    t_export *head_exp = export;

    while (head_exp != NULL) {
        if (strcmp(head_exp->name, name) == 0) {
            flag++;
            free(head_exp->value);
            head_exp->value = strdup(value);
            break;
        }
        head_exp = head_exp->next;
    }
    if (!flag) mx_push_export(&export, name, value);
}

static void get_data (char *arg, char **name, char **value, t_export *var) {
    int indx = mx_get_char_index(arg,'=');

    if (indx < 0){
        *name = strdup(arg);
    } else {
        *name = strndup(arg,indx);
        *value = mx_strdup_from(arg,indx);
    }
    if (*value == NULL) {
        t_export *head_exp = var;
        while (head_exp != NULL) {
            if (strcmp(head_exp->name, *name) == 0) {
                *value = strdup(head_exp->value);
                break;
            }
            head_exp = head_exp->next;
        }
    }
}

int add_parameter(char *param, t_export **env_params, char option) {
    char *tmp_option = malloc(2*sizeof(char));;

    if (param) {
        if (mx_strchr(param, '=') && option == 'u') {
            mx_printerr("env: unsetenv ");
            mx_printerr(param);
            mx_printerr(": Invalid argument\n");
            return -1;
        }
        tmp_option[0] = option;
        tmp_option[1] = '\0';
        mx_push_export(env_params, tmp_option, param);
        free(param);
        free(tmp_option);
        return 0;
    } else {
        mx_print_env_error(option, "env: option requires an argument -- ");
        return -1;
    }
}

void mx_export_or_error(char *arg, t_export *export, t_export *var, int *exit_index) {
    int flag = check_identifier(arg);

    if (flag){
        print_export_error(arg, exit_index);
    } else {
        char *name = NULL;
        char *value = NULL;
        get_data(arg, &name, &value, var);
        if (value != NULL) setenv(name, value, 1);
        export_value(export, name, value);
        export_value(var, name, value);
        mx_clear_data(name, value);
        *exit_index = 0;
    }
}

static t_export *create_node(void *name, void *value) {
    t_export *node =  (t_export *)malloc(sizeof(t_export));

    node->name = strdup(name);
    if(!value)
        node->value = strdup("");
    else
        node->value = strdup(value);
    node->next = NULL;
    return node;
}

void mx_push_export(t_export **list, void *name, void *value) {
    t_export *temp;
    t_export *export_p;

    if (!list) {
        return;
    }
    
    temp = create_node(name, value);
    if (!temp) {
        return;
    }

    export_p = *list;
    if (*list == NULL) {
        *list = temp;
        return;
    }
    else {
        while (export_p->next != NULL) {
            export_p = export_p->next;
        }
        export_p->next = temp;
    }
}