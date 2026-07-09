#include <pthread.h>
#include <stdlib.h>
#include "multithreading.h"

#define MAX_THREADS 16

/**
 * blur_thread - Thread routine for blurring a portion of an image.
 * @arg: Portion description.
 *
 * Return: NULL.
 */
static void *blur_thread(void *arg)
{
	blur_portion((blur_portion_t const *)arg);
	return (NULL);
}

/**
 * blur_image - Blur an image using multiple threads.
 * @img_blur: Destination image.
 * @img: Source image.
 * @kernel: Convolution kernel.
 */
void blur_image(img_t *img_blur, img_t const *img, kernel_t const *kernel)
{
	pthread_t threads[MAX_THREADS];
	blur_portion_t portions[MAX_THREADS];
	size_t thread_count;
	size_t rows_per_thread;
	size_t remainder;
	size_t i;
	size_t start;
	void *thread_result;

	if (img_blur == NULL || img == NULL || kernel == NULL || img->h == 0)
		return;
	thread_count = img->h < MAX_THREADS ? img->h : MAX_THREADS;
	if (thread_count == 0)
		return;
	rows_per_thread = img->h / thread_count;
	remainder = img->h % thread_count;
	start = 0;
	for (i = 0; i < thread_count; i++)
	{
		portions[i].img = img;
		portions[i].img_blur = img_blur;
		portions[i].kernel = kernel;
		portions[i].x = 0;
		portions[i].y = start;
		portions[i].h = rows_per_thread + (i < remainder ? 1 : 0);
		portions[i].w = img->w;
		pthread_create(&threads[i], NULL, blur_thread, &portions[i]);
		start += portions[i].h;
	}
	for (i = 0; i < thread_count; i++)
		pthread_join(threads[i], &thread_result);
}
