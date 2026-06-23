#ifndef HNM_H
#define HNM_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <string.h>

int process_64(const char *filename, char *map, Elf64_Ehdr *ehdr);
int process_32(const char *filename, char *map, Elf32_Ehdr *ehdr);
int parse_elf(const char *filename);

#endif /* HNM_H */
