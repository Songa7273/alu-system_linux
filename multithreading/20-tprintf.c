#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include "multithreading.h"

static pthread_mutex_t logger_lock;

__attribute__((constructor)) static void init_logger_lock(void)
{
	pthread_mutex_init(&logger_lock, NULL);
}

__attribute__((destructor)) static void destroy_logger_lock(void)
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
