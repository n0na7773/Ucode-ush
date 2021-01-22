#include "libmx.h"

t_list *mx_create_node(void *data) {
    if (data == NULL) return NULL;
    t_list *node = (t_list *) malloc(sizeof(t_list));
    if (node == NULL) return NULL;
    node->data = mx_strdup((char *)data);
    node->next = NULL;
    return node;
}
