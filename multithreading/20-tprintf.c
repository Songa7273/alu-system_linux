#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include "multithreading.h"

static pthread_mutex_t logger_lock;

/**
 * init_logger_lock - Initialize the logger mutex before program start.
 */
static void __attribute__((constructor)) init_logger_lock(void)
{
	pthread_mutex_init(&logger_lock, NULL);
}

/**
 * destroy_logger_lock - Destroy the logger mutex when the program exits.
 */
static void __attribute__((destructor)) destroy_logger_lock(void)
{
	pthread_mutex_destroy(&logger_lock);
}

/**
 * tprintf - Print a formatted string with the thread ID prefix.
 * @format: Format string.
 *
 * Return: Number of characters printed.
 */
int tprintf(char const *format, ...)
{
	va_list args;
	int count;

	pthread_mutex_lock(&logger_lock);
	count = printf("[%lu] ", (unsigned long)pthread_self());
	va_start(args, format);
	count += vprintf(format, args);
	va_end(args);
	pthread_mutex_unlock(&logger_lock);
	return (count);
}
