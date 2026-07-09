#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include "multithreading.h"

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

	count = printf("[%lu] ", (unsigned long)pthread_self());
	va_start(args, format);
	count += vprintf(format, args);
	va_end(args);
	return (count);
}
