#include "hobjdump.h"

/**
 * loop_sections_32 - Helper to process and print 32-bit sections
 * @shnum: Number of sections
 * @shdr: Pointer to section header array
 * @shstrtab: Section string table pointer
 * @map: Base memory map pointer
 * @bfd_be: Big endian format toggle state flag
 */
static void loop_sections_32(uint16_t shnum, Elf32_Shdr *shdr,
			     char *shstrtab, char *map, int bfd_be)
{
	uint16_t i;

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
	int bfd_be = (ehdr->e_ident[EI_DATA] == ELFDATA2MSB);
	uint16_t shnum = SWAP16(ehdr->e_shnum, bfd_be);
	uint16_t shstrndx = SWAP16(ehdr->e_shstrndx, bfd_be);
	Elf32_Shdr *shdr = (Elf32_Shdr *)(map + SWAP32(ehdr->e_shoff, bfd_be));
	char *shstrtab = map + SWAP32(shdr[shstrndx].sh_offset, bfd_be);
	uint16_t type_e = SWAP16(ehdr->e_type, bfd_be);
	uint16_t mach = SWAP16(ehdr->e_machine, bfd_be);
	uint32_t flags = 0, i;
	char fmt[64], arch[64];

	get_elf_target(mach, fmt, arch);
	if (type_e == ET_REL)
		flags |= HAS_RELOC;
	if (type_e == ET_EXEC)
		flags |= EXEC_P;
	if (type_e == ET_DYN)
		flags |= DYNAMIC;
	for (i = 0; i < shnum; i++)
	{
		uint32_t type = SWAP32(shdr[i].sh_type, bfd_be);

		if (type == SHT_SYMTAB)
			flags |= HAS_SYMS;
		if (type == SHT_DYNAMIC || type_e == ET_EXEC || type_e == ET_DYN)
			flags |= D_PAGED;
	}
	printf("\n%s:     file format %s\n", filename, fmt);
	printf("architecture: %s, flags 0x%08x:\n", arch, flags);
	print_flags_string(flags);
	printf("start address 0x%08lx\n\n",
	       (unsigned long)SWAP32(ehdr->e_entry, bfd_be));
	loop_sections_32(shnum, shdr, shstrtab, map, bfd_be);
	return (0);
}
