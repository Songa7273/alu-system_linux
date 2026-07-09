#ifndef LIST_H
#define LIST_H

#include <stddef.h>

struct node_s;
struct list_s;

typedef struct node_s node_t;
typedef struct list_s list_t;
typedef void (*node_func_t)(void *);

typedef struct node_s
{
	void *content;
	node_t *prev;
	node_t *next;
} node_t;

typedef struct list_s
{
	size_t size;
	node_t *head;
	node_t *tail;
} list_t;

list_t *list_init(list_t *list);
node_t *list_add(list_t *list, void *content);
void list_each(list_t const *list, node_func_t func);
void list_destroy(list_t *list, node_func_t func);

#endif
