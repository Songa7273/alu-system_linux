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
	task_t *selected;
	void *result;
	size_t index;
	size_t selected_index;
	int found;

	if (tasks == NULL)
		return (NULL);
	while (1)
	{
		found = 0;
		selected = NULL;
		selected_index = 0;
		pthread_mutex_lock(&task_list_lock);
		for (index = 0, node = tasks->head; node != NULL; node = node->next, index++)
		{
			task = node->content;
			pthread_mutex_lock(&task->lock);
			if (task->status == PENDING)
			{
				task->status = STARTED;
				selected = task;
				selected_index = index;
				found = 1;
				pthread_mutex_unlock(&task->lock);
				break;
			}
			pthread_mutex_unlock(&task->lock);
		}
		pthread_mutex_unlock(&task_list_lock);
		if (found == 0)
			break;
		tprintf("[%02lu] Started\n", (unsigned long)selected_index);
		result = selected->entry(selected->param);
		pthread_mutex_lock(&selected->lock);
		selected->result = result;
		selected->status = (result == NULL) ? FAILURE : SUCCESS;
		pthread_mutex_unlock(&selected->lock);
		tprintf("[%02lu] %s\n", (unsigned long)selected_index,
			selected->status == SUCCESS ? "Success" : "Failure");
	}
	return (NULL);
}
