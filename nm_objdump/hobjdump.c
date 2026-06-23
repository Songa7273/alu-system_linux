#include "hobjdump.h"

/**
 * print_section_contents - Prints contents of a section in objdump format
 * @name: Name of the section
 * @start: Pointer to section data start in memory
 * @size: Size of the section data
 * @vma: Virtual memory address of the section
 */
void print_section_contents(const char *name, char *start,
			     size_t size, uint64_t vma)
{
	size_t i, j;
	unsigned char c;

	printf("Contents of section %s:\n", name);
	for (i = 0; i < size; i += 16)
	{
		printf(" %04lx ", (unsigned long)(vma + i));
		for (j = 0; j < 16; j++)
		{
			if (i + j < size)
				printf("%02x", (unsigned char)start[i + j]);
			else
				printf("  ");
			if ((j + 1) % 4 == 0)
				printf(" ");
		}
		printf(" ");
		for (j = 0; j < 16; j++)
		{
			if (i + j < size)
			{
				c = start[i + j];
				printf("%c", isprint(c) ? c : '.');
			}
			else
				printf(" ");
		}
		printf("\n");
	}
}

/**
 * get_elf_target - Resolves architecture and file format target strings
 * @machine: ELF machine identifier field
 * @is_64: 1 if 64-bit class layout, 0 if 32-bit layout
 * @fmt: Destination buffer for file format representation
 * @arch: Destination buffer for target architecture representation
 */
void get_elf_target(uint16_t machine, int is_64, char *fmt, char *arch)
{
	if (machine == EM_X86_64)
	{
		strcpy(fmt, "elf64-x86-64");
		strcpy(arch, "i386:x86-64");
	}
	else if (machine == EM_386)
	{
		strcpy(fmt, "elf32-i386");
		strcpy(arch, "i386");
	}
	else if (machine == EM_SPARC || machine == EM_SPARC32PLUS ||
		 machine == EM_SPARCV9)
	{
		strcpy(fmt, is_64 ? "elf64-sparc" : "elf32-sparc");
		strcpy(arch, "sparc");
	}
	else
	{
		strcpy(fmt, is_64 ? "elf64-unknown" : "elf32-unknown");
		strcpy(arch, "unknown");
	}
}

/**
 * process_obj_64 - Deep parses sections for 64-bit ELF layouts
 * @filename: Targeted operational filename
 * @map: Base memory map pointer
 * @ehdr: Native pointer mapping the target ELF structure
 * Return: 0 on successful processing, 1 on malformed layouts
 */
int process_obj_64(const char *filename, char *map, Elf64_Ehdr *ehdr)
{
	uint16_t shnum = SWAP16(ehdr->e_shnum, bfd_be);
	uint16_t shstrndx = SWAP16(ehdr->e_shstrndx, bfd_be);
	Elf64_Shdr *shdr = (Elf64_Shdr *)(map + SWAP64(ehdr->e_shoff, bfd_be));
	char *shstrtab = map + SWAP64(shdr[shstrndx].sh_offset, bfd_be);
	uint16_t type_e = SWAP16(ehdr->e_type, bfd_be);
	uint16_t mach = SWAP16(ehdr->e_machine, bfd_be);
	uint32_t flags = HAS_SYMS | D_PAGED;
	char fmt[64], arch[64];
	uint16_t i;

	get_elf_target(mach, 1, fmt, arch);
	flags |= (type_e == ET_EXEC) ? EXEC_P : DYNAMIC;
	printf("\n%s:     file format %s\n", filename, fmt);
	printf("architecture: %s, flags 0x%08x:\n", arch, flags);
	if (type_e == ET_EXEC)
		printf("EXEC_P, HAS_SYMS, D_PAGED\n");
	else
		printf("HAS_SYMS, DYNAMIC, D_PAGED\n");
	printf("start address 0x%016lx\n\n",
	       (unsigned long)SWAP64(ehdr->e_entry, bfd_be));

	for (i = 0; i < shnum; i++)
	{
		char *name = shstrtab + SWAP32(shdr[i].sh_name, bfd_be);
		uint32_t type = SWAP32(shdr[i].sh_type, bfd_be);
		uint64_t size = SWAP64(shdr[i].sh_size, bfd_be);
		uint64_t offset = SWAP64(shdr[i].sh_offset, bfd_be);
		uint64_t vma = SWAP64(shdr[i].sh_addr, bfd_be);

		if (type == SHT_NOBITS || type == SHT_NULL || !size ||
		    strcmp(name, ".strtab") == 0 || strcmp(name, ".symtab") == 0 ||
		    strcmp(name, ".shstrtab") == 0)
			continue;
		print_section_contents(name, map + offset, size, vma);
	}
	return (0);
}

/**
 * process_obj_32 - Deep parses sections for 32-bit ELF layouts
 * @filename: Targeted operational filename
 * @map: Base memory map pointer
 * @ehdr: Native pointer mapping the target ELF structure
 * Return: 0 on successful processing, 1 on malformed layouts
 */
int process_obj_32(const char *filename, char *map, Elf32_Ehdr *ehdr)
{
	uint16_t shnum = SWAP16(ehdr->e_shnum, bfd_be);
	uint16_t shstrndx = SWAP16(ehdr->e_shstrndx, bfd_be);
	Elf32_Shdr *shdr = (Elf32_Shdr *)(map + SWAP32(ehdr->e_shoff, bfd_be));
	char *shstrtab = map + SWAP32(shdr[shstrndx].sh_offset, bfd_be);
	uint16_t type_e = SWAP16(ehdr->e_type, bfd_be);
	uint16_t mach = SWAP16(ehdr->e_machine, bfd_be);
	uint32_t flags = HAS_SYMS | D_PAGED;
	char fmt[64], arch[64];
	uint16_t i;

	get_elf_target(mach, 0, fmt, arch);
	flags |= (type_e == ET_EXEC) ? EXEC_P : DYNAMIC;
	printf("\n%s:     file format %s\n", filename, fmt);
	printf("architecture: %s, flags 0x%08x:\n", arch, flags);
	if (type_e == ET_EXEC)
		printf("EXEC_P, HAS_SYMS, D_PAGED\n");
	else
		printf("HAS_SYMS, DYNAMIC, D_PAGED\n");
	printf("start address 0x%08lx\n\n",
	       (unsigned long)SWAP32(ehdr->e_entry, bfd_be));

	for (i = 0; i < shnum; i++)
	{
		char *name = shstrtab + SWAP32(shdr[i].sh_name, bfd_be);
		uint32_t type = SWAP32(shdr[i].sh_type, bfd_be);
		uint32_t size = SWAP32(shdr[i].sh_size, bfd_be);
		uint32_t offset = SWAP32(shdr[i].sh_offset, bfd_be);
		uint32_t vma = SWAP32(shdr[i].sh_addr, bfd_be);

		if (type == SHT_NOBITS || type == SHT_NULL || !size ||
		    strcmp(name, ".strtab") == 0 || strcmp(name, ".symtab") == 0 ||
		    strcmp(name, ".shstrtab") == 0)
			continue;
		print_section_contents(name, map + offset, size, vma);
	}
	return (0);
}
