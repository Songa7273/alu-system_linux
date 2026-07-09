#include <stddef.h>
#include <stdlib.h>
#include "multithreading.h"

static float kernel_sum(kernel_t const *kernel)
{
	size_t i;
	size_t j;
	float sum;

	sum = 0.0f;
	for (i = 0; i < kernel->size; i++)
	{
		for (j = 0; j < kernel->size; j++)
			sum += kernel->matrix[i][j];
	}
	return (sum);
}

static void accumulate_channel(img_t const *img, kernel_t const *kernel,
	size_t x, size_t y, float *red, float *green, float *blue)
{
	size_t half;
	size_t i;
	size_t j;
	long px;
	long py;
	float weight;

	half = kernel->size / 2;
	for (i = 0; i < kernel->size; i++)
	{
		for (j = 0; j < kernel->size; j++)
		{
			px = (long)x + (long)j - (long)half;
			py = (long)y + (long)i - (long)half;
			if (px < 0 || py < 0 || px >= (long)img->w || py >= (long)img->h)
				continue;
			weight = kernel->matrix[i][j];
			*red += img->pixels[py * (long)img->w + px].r * weight;
			*green += img->pixels[py * (long)img->w + px].g * weight;
			*blue += img->pixels[py * (long)img->w + px].b * weight;
		}
	}
}

static pixel_t blur_pixel(img_t const *img, kernel_t const *kernel, size_t x,
	size_t y)
{
	float sum;
	float red;
	float green;
	float blue;
	pixel_t pixel;

	sum = kernel_sum(kernel);
	red = 0.0f;
	green = 0.0f;
	blue = 0.0f;
	accumulate_channel(img, kernel, x, y, &red, &green, &blue);
	if (sum != 0.0f)
	{
		red /= sum;
		green /= sum;
		blue /= sum;
	}
	if (red < 0.0f)
		red = 0.0f;
	if (green < 0.0f)
		green = 0.0f;
	if (blue < 0.0f)
		blue = 0.0f;
	if (red > 255.0f)
		red = 255.0f;
	if (green > 255.0f)
		green = 255.0f;
	if (blue > 255.0f)
		blue = 255.0f;
	pixel.r = (uint8_t)red;
	pixel.g = (uint8_t)green;
	pixel.b = (uint8_t)blue;
	return (pixel);
}

/**
 * blur_portion - Blur a portion of an image.
 * @portion: Portion description.
 */
void blur_portion(blur_portion_t const *portion)
{
	size_t x;
	size_t y;

	if (portion == NULL || portion->img == NULL || portion->img_blur == NULL ||
		portion->kernel == NULL)
		return;
	for (y = portion->y; y < portion->y + portion->h && y < portion->img->h;
		y++)
	{
		for (x = portion->x; x < portion->x + portion->w && x < portion->img->w;
			x++)
			portion->img_blur->pixels[y * portion->img_blur->w + x] =
				blur_pixel(portion->img, portion->kernel, x, y);
	}
}
