#include <stdio.h>
#include "multithreading.h"

/**
 * thread_entry - Thread entry point.
 * @arg: Address of a string to print.
 *
 * Return: NULL.
 */
void *thread_entry(void *arg)
{
	printf("%s\n", (char *)arg);
	return (NULL);
}
