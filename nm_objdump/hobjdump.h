#ifndef HOBJDUMP_H
#define HOBJDUMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <elf.h>

/* GNU BFD Flags */
#define HAS_RELOC     0x01
#define EXEC_P        0x02
#define HAS_SYMS      0x10
#define DYNAMIC       0x40
#define D_PAGED       0x100

/* Endianness tracking */
#define SWAP16(val, be) ((be) ? __builtin_bswap16(val) : (val))
#define SWAP32(val, be) ((be) ? __builtin_bswap32(val) : (val))
#define SWAP64(val, be) ((be) ? __builtin_bswap64(val) : (val))

/* Function Prototypes */
void print_section_contents(const char *name, char *start,
			     size_t size, uint64_t vma);
void get_elf_target(uint16_t machine, char *fmt, char *arch);
void print_flags_string(uint32_t flags);
int process_obj_64(const char *filename, char *map, Elf64_Ehdr *ehdr);
int process_obj_32(const char *filename, char *map, Elf32_Ehdr *ehdr);

#endif /* HOBJDUMP_H */
