#include <stdlib.h>
#include "list.h"

/**
 * list_init - Initialize a list.
 * @list: List to initialize.
 *
 * Return: The initialized list, or NULL on failure.
 */
list_t *list_init(list_t *list)
{
	if (list == NULL)
		return (NULL);
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
	return (list);
}

/**
 * list_add - Add a node to the end of a list.
 * @list: List to update.
 * @content: Node content.
 *
 * Return: The added node, or NULL on failure.
 */
node_t *list_add(list_t *list, void *content)
{
	node_t *node;

	if (list == NULL)
		return (NULL);
	node = malloc(sizeof(*node));
	if (node == NULL)
		return (NULL);
	node->content = content;
	node->next = NULL;
	node->prev = list->tail;
	if (list->tail != NULL)
		list->tail->next = node;
	else
		list->head = node;
	list->tail = node;
	list->size++;
	return (node);
}

/**
 * list_each - Apply a function to each node content.
 * @list: List to traverse.
 * @func: Function to apply.
 */
void list_each(list_t const *list, node_func_t func)
{
	node_t *node;

	if (list == NULL || func == NULL)
		return;
	for (node = list->head; node != NULL; node = node->next)
		func(node->content);
}

/**
 * list_destroy - Destroy a list.
 * @list: List to destroy.
 * @func: Destructor for node content.
 */
void list_destroy(list_t *list, node_func_t func)
{
	node_t *node;
	node_t *next;

	if (list == NULL)
		return;
	node = list->head;
	while (node != NULL)
	{
		next = node->next;
		if (func != NULL)
			func(node->content);
		free(node);
		node = next;
	}
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
}
