#include <pthread.h>
#include <stdlib.h>
#include "multithreading.h"

#define NB_THREADS 8

static pthread_mutex_t task_list_lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * create_task - Create a task.
 * @entry: Task entry point.
 * @param: Task parameter.
 *
 * Return: Created task, or NULL on failure.
 */
task_t *create_task(task_entry_t entry, void *param)
{
	task_t *task;

	task = malloc(sizeof(*task));
	if (task == NULL)
		return (NULL);
	task->entry = entry;
	task->param = param;
	task->status = PENDING;
	task->result = NULL;
	if (pthread_mutex_init(&task->lock, NULL) != 0)
	{
		free(task);
		return (NULL);
	}
	return (task);
}

/**
 * destroy_task - Destroy a task.
 * @task: Task to destroy.
 */
void destroy_task(task_t *task)
{
	if (task == NULL)
		return;
	if (task->result != NULL)
	{
		list_destroy((list_t *)task->result, free);
		free(task->result);
	}
	pthread_mutex_destroy(&task->lock);
	free(task);
}

/**
 * exec_tasks - Execute a list of tasks.
 * @tasks: Task list.
 *
 * Return: NULL.
 */
void *exec_tasks(list_t const *tasks)
{
	node_t *node;
	task_t *task;
	void *result;
	size_t index;
	int found;

	if (tasks == NULL)
		return (NULL);
	while (1)
	{
		found = 0;
		pthread_mutex_lock(&task_list_lock);
		for (index = 0, node = tasks->head; node != NULL; node = node->next, index++)
		{
			task = node->content;
			pthread_mutex_lock(&task->lock);
			if (task->status == PENDING)
			{
				task->status = STARTED;
				pthread_mutex_unlock(&task->lock);
				pthread_mutex_unlock(&task_list_lock);
				tprintf("[%02lu] Started\n", (unsigned long)index);
				result = task->entry(task->param);
				pthread_mutex_lock(&task->lock);
				task->result = result;
				task->status = SUCCESS;
				pthread_mutex_unlock(&task->lock);
				tprintf("[%02lu] Success\n", (unsigned long)index);
				found = 1;
				break;
			}
			pthread_mutex_unlock(&task->lock);
		}
		pthread_mutex_unlock(&task_list_lock);
		if (found == 0)
			break;
	}
	return (NULL);
}
