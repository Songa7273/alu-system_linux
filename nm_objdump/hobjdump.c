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
 * @fmt: Destination buffer for file format representation
 * @arch: Destination buffer for target architecture representation
 */
void get_elf_target(uint16_t machine, char *fmt, char *arch)
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
		strcpy(fmt, "elf32-big");
		strcpy(arch, "UNKNOWN!");
	}
	else
	{
		strcpy(fmt, "elf64-unknown");
		strcpy(arch, "unknown");
	}
}

/**
 * print_flags_string - Helper to format objdump flags cleanly
 * @flags: Computed flags bitfield
 */
void print_flags_string(uint32_t flags)
{
	int printed = 0;

	if (flags & HAS_RELOC)
	{
		printf("HAS_RELOC");
		printed = 1;
	}
	if (flags & EXEC_P)
	{
		printf("%sEXEC_P", printed ? ", " : "");
		printed = 1;
	}
	if (flags & HAS_SYMS)
	{
		printf("%sHAS_SYMS", printed ? ", " : "");
		printed = 1;
	}
	if (flags & DYNAMIC)
	{
		printf("%sDYNAMIC", printed ? ", " : "");
		printed = 1;
	}
	if (flags & D_PAGED)
	{
		printf("%sD_PAGED", printed ? ", " : "");
	}
	printf("\n");
}

/**
 * loop_sections_64 - Helper to process and print 64-bit sections
 * @shnum: Number of sections
 * @shdr: Pointer to section header array
 * @shstrtab: Section string table pointer
 * @map: Base memory map pointer
 * @bfd_be: Big endian format toggle state flag
 */
static void loop_sections_64(uint16_t shnum, Elf64_Shdr *shdr,
			     char *shstrtab, char *map, int bfd_be)
{
	uint16_t i;

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
	int bfd_be = (ehdr->e_ident[EI_DATA] == ELFDATA2MSB);
	uint16_t shnum = SWAP16(ehdr->e_shnum, bfd_be);
	uint16_t shstrndx = SWAP16(ehdr->e_shstrndx, bfd_be);
	Elf64_Shdr *shdr = (Elf64_Shdr *)(map + SWAP64(ehdr->e_shoff, bfd_be));
	char *shstrtab = map + SWAP64(shdr[shstrndx].sh_offset, bfd_be);
	uint16_t type_e = SWAP16(ehdr->e_type, bfd_be);
	uint16_t mach = SWAP16(ehdr->e_machine, bfd_be);
	uint32_t flags = 0, i;
	char fmt[64], arch[64];

	get_elf_target(mach, fmt, arch);

	if (type_e == ET_REL)
		flags |= HAS_RELOC;
	else if (type_e == ET_EXEC)
		flags |= (EXEC_P | D_PAGED);
	else if (type_e == ET_DYN)
		flags |= (DYNAMIC | D_PAGED);

	for (i = 0; i < shnum; i++)
	{
		uint32_t type = SWAP32(shdr[i].sh_type, bfd_be);

		if (type == SHT_SYMTAB)
			flags |= HAS_SYMS;
	}

	printf("\n%s:     file format %s\n", filename, fmt);
	printf("architecture: %s, flags 0x%08x:\n", arch, flags);
	print_flags_string(flags);
	printf("start address 0x%016lx\n\n",
	       (unsigned long)SWAP64(ehdr->e_entry, bfd_be));
	loop_sections_64(shnum, shdr, shstrtab, map, bfd_be);
	return (0);
}
