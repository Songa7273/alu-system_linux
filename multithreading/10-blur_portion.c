#include <stddef.h>
#include <stdlib.h>
#include "multithreading.h"

static pixel_t blur_pixel(img_t const *img, kernel_t const *kernel, size_t x,
	size_t y)
{
	size_t half;
	size_t i;
	size_t j;
	float kernel_sum;
	float red;
	float green;
	float blue;
	long px;
	long py;
	float weight;
	pixel_t pixel;

	half = kernel->size / 2;
	kernel_sum = 0.0f;
	red = 0.0f;
	green = 0.0f;
	blue = 0.0f;
	for (i = 0; i < kernel->size; i++)
	{
		for (j = 0; j < kernel->size; j++)
		{
			kernel_sum += kernel->matrix[i][j];
			px = (long)x + (long)j - (long)half;
			py = (long)y + (long)i - (long)half;
			if (px < 0 || py < 0 || px >= (long)img->w || py >= (long)img->h)
				continue;
			weight = kernel->matrix[i][j];
			red += img->pixels[py * (long)img->w + px].r * weight;
			green += img->pixels[py * (long)img->w + px].g * weight;
			blue += img->pixels[py * (long)img->w + px].b * weight;
		}
	}
	if (kernel_sum != 0.0f)
	{
		red /= kernel_sum;
		green /= kernel_sum;
		blue /= kernel_sum;
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
	pixel.r = (uint8_t)(red + 0.5f);
	pixel.g = (uint8_t)(green + 0.5f);
	pixel.b = (uint8_t)(blue + 0.5f);
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
