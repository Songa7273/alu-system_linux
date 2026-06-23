#ifndef HOBJDUMP_H
#define HOBJDUMP_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define EXEC_P         0x02
#define HAS_SYMS       0x10
#define DYNAMIC        0x40
#define D_PAGED        0x100

#define bfd_be (map[EI_DATA] == ELFDATA2MSB)

#define SWAP16(val, be) \
	((!be) ? (val) : (uint16_t)(((val) >> 8) | ((val) << 8)))

#define SWAP32(val, be) \
	((!be) ? (val) : (uint32_t)( \
		(((val) >> 24) & 0xff) | \
		(((val) >> 8) & 0xff00) | \
		(((val) << 8) & 0xff0000) | \
		(((val) << 24) & 0xff000000)))

#define SWAP64(val, be) \
	((!be) ? (val) : (uint64_t)( \
		(((val) >> 56) & 0xff) | \
		(((val) >> 40) & 0xff00) | \
		(((val) >> 24) & 0xff0000) | \
		(((val) >> 8) & 0xff000000) | \
		(((val) << 8) & 0xff00000000ULL) | \
		(((val) << 24) & 0xff0000000000ULL) | \
		(((val) << 40) & 0xff000000000000ULL) | \
		(((val) << 56) & 0xff00000000000000ULL)))

int process_obj_64(const char *filename, char *map, Elf64_Ehdr *ehdr);
int process_obj_32(const char *filename, char *map, Elf32_Ehdr *ehdr);

#endif /* HOBJDUMP_H */
