#include <stdlib.h>
#include <string.h>
#include "multithreading.h"
#include "list.h"

/**
 * parse_number - Convert a string of digits to an unsigned long.
 * @s: String representation of the number.
 *
 * Return: The parsed unsigned long value.
 */
static unsigned long parse_number(char const *s)
{
	unsigned long number;

	number = 0;
	while (s != NULL && *s >= '0' && *s <= '9')
	{
		number = number * 10 + (unsigned long)(*s - '0');
		s++;
	}
	return (number);
}

/**
 * prime_factors - Factorize a positive number into prime factors.
 * @s: Number as a string.
 *
 * Return: A list of prime factors.
 */
list_t *prime_factors(char const *s)
{
	list_t *factors;
	unsigned long number;
	unsigned long divisor;
	unsigned long *factor;

	if (s == NULL)
		return (NULL);
	factors = malloc(sizeof(*factors));
	if (factors == NULL)
		return (NULL);
	list_init(factors);
	number = parse_number(s);
	divisor = 2;
	while (number > 1 && divisor * divisor <= number)
	{
		while (number % divisor == 0)
		{
			factor = malloc(sizeof(*factor));
			if (factor == NULL)
				return (factors);
			*factor = divisor;
			list_add(factors, factor);
			number /= divisor;
		}
		divisor++;
	}
	if (number > 1)
	{
		factor = malloc(sizeof(*factor));
		if (factor != NULL)
		{
			*factor = number;
			list_add(factors, factor);
		}
	}
	return (factors);
}
