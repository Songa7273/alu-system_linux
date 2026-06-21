#include <stdio.h>
#include "elf_parser.h"

int main(int argc, char **argv)
{
    int i;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <objfile ...>\n", argv[0]);
        return (1);
    }

    for (i = 1; i < argc; i++)
        parse_elf(argv[i]);

    return (0);
}
