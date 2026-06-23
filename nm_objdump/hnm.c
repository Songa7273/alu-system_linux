#include "hnm.h"

/**
 * get_type_64 - Determines the nm char type for a 64-bit ELF symbol
 * @sym: Pointer to Elf64_Sym
 * @shdr: Pointer to section headers
 * @shnum: Number of sections
 * @be: Big-endian flag
 * Return: Character flag representing the type
 */
char get_type_64(Elf64_Sym *sym, Elf64_Shdr *shdr, int shnum, int be)
{
	char c = '?';
	uint16_t shndx = SWAP16(sym->st_shndx, be);
	uint32_t type, flags;

	if (ELF64_ST_BIND(sym->st_info) == STB_WEAK)
	{
		c = (shndx == SHN_UNDEF) ? 'w' : 'W';
		if (ELF64_ST_TYPE(sym->st_info) == STT_OBJECT)
			c = (shndx == SHN_UNDEF) ? 'v' : 'V';
		return (c);
	}
	if (shndx == SHN_UNDEF)
		return ('U');
	if (shndx == SHN_ABS)
		c = 'A';
	else if (shndx == SHN_COMMON)
		c = 'C';
	else if (shndx < shnum)
	{
		type = SWAP32(shdr[shndx].sh_type, be);
		flags = SWAP64(shdr[shndx].sh_flags, be);

		if (type == SHT_NOBITS)
			c = 'B';
		else if (type == SHT_INIT_ARRAY || type == SHT_FINI_ARRAY)
			c = 'T';
		else if (flags & SHF_WRITE)
			c = 'D';
		else if (flags & SHF_EXECINSTR)
			c = 'T';
		else
			c = 'R';
	}
	if (ELF64_ST_BIND(sym->st_info) == STB_LOCAL && c != '?')
		c += 32;
	return (c);
}

/**
 * process_64 - Processes a 64-bit ELF file
 * @filename: Name of the file
 * @map: Memory mapped pointer to the file start
 * @ehdr: Pointer to ELF header
 * Return: 0 on success, 1 on failure
 */
int process_64(const char *filename, char *map, Elf64_Ehdr *ehdr)
{
	int be = (map[EI_DATA] == ELFDATA2MSB);
	Elf64_Shdr *shdr = (Elf64_Shdr *)(map + SWAP64(ehdr->e_shoff, be));
	Elf64_Shdr *symtab_sh = NULL;
	Elf64_Sym *syms;
	char *strtab;
	int i, shnum = SWAP16(ehdr->e_shnum, be), num_syms;

	for (i = 0; i < shnum; i++)
	{
		if (SWAP32(shdr[i].sh_type, be) == SHT_SYMTAB)
		{
			symtab_sh = &shdr[i];
			break;
		}
	}
	if (!symtab_sh)
	{
		fprintf(stderr, "./hnm: %s: no symbols\n", filename);
		return (1);
	}
	syms = (Elf64_Sym *)(map + SWAP64(symtab_sh->sh_offset, be));
	num_syms = SWAP64(symtab_sh->sh_size, be) / SWAP64(symtab_sh->sh_entsize, be);
	strtab = map + SWAP64(shdr[SWAP32(symtab_sh->sh_link, be)].sh_offset, be);

	for (i = 0; i < num_syms; i++)
	{
		int type = ELF64_ST_TYPE(syms[i].st_info);
		uint32_t name = SWAP32(syms[i].st_name, be);
		char c;

		if (!name || type == STT_FILE || type == STT_SECTION)
			continue;
		c = get_type_64(&syms[i], shdr, shnum, be);
		if (c == 'U' || c == 'w' || c == 'v')
			printf("                 %c %s\n", c, strtab + name);
		else
			printf("%016lx %c %s\n", SWAP64(syms[i].st_value, be), c, strtab + name);
	}
	return (0);
}

/**
 * get_type_32 - Determines the nm char type for a 32-bit ELF symbol
 * @sym: Pointer to Elf32_Sym
 * @shdr: Pointer to section headers
 * @shnum: Number of sections
 * @be: Big-endian flag
 * Return: Character flag representing the type
 */
char get_type_32(Elf32_Sym *sym, Elf32_Shdr *shdr, int shnum, int be)
{
	char c = '?';
	uint16_t shndx = SWAP16(sym->st_shndx, be);
	uint32_t type, flags;

	if (ELF32_ST_BIND(sym->st_info) == STB_WEAK)
	{
		c = (shndx == SHN_UNDEF) ? 'w' : 'W';
		if (ELF32_ST_TYPE(sym->st_info) == STT_OBJECT)
			c = (shndx == SHN_UNDEF) ? 'v' : 'V';
		return (c);
	}
	if (shndx == SHN_UNDEF)
		return ('U');
	if (shndx == SHN_ABS)
		c = 'A';
	else if (shndx == SHN_COMMON)
		c = 'C';
	else if (shndx < shnum)
	{
		type = SWAP32(shdr[shndx].sh_type, be);
		flags = SWAP32(shdr[shndx].sh_flags, be);

		if (type == SHT_NOBITS)
			c = 'B';
		else if (type == SHT_INIT_ARRAY || type == SHT_FINI_ARRAY)
			c = 'T';
		else if (flags & SHF_WRITE)
			c = 'D';
		else if (flags & SHF_EXECINSTR)
			c = 'T';
		else
			c = 'R';
	}
	if (ELF32_ST_BIND(sym->st_info) == STB_LOCAL && c != '?')
		c += 32;
	return (c);
}

/**
 * process_32 - Processes a 32-bit ELF file
 * @filename: Name of the file
 * @map: Memory mapped pointer to the file start
 * @ehdr: Pointer to ELF header
 * Return: 0 on success, 1 on failure
 */
int process_32(const char *filename, char *map, Elf32_Ehdr *ehdr)
{
	int be = (map[EI_DATA] == ELFDATA2MSB);
	Elf32_Shdr *shdr = (Elf32_Shdr *)(map + SWAP32(ehdr->e_shoff, be));
	Elf32_Shdr *symtab_sh = NULL;
	Elf32_Sym *syms;
	char *strtab;
	int i, shnum = SWAP16(ehdr->e_shnum, be), num_syms;

	for (i = 0; i < shnum; i++)
	{
		if (SWAP32(shdr[i].sh_type, be) == SHT_SYMTAB)
		{
			symtab_sh = &shdr[i];
			break;
		}
	}
	if (!symtab_sh)
	{
		fprintf(stderr, "./hnm: %s: no symbols\n", filename);
		return (1);
	}
	syms = (Elf32_Sym *)(map + SWAP32(symtab_sh->sh_offset, be));
	num_syms = SWAP32(symtab_sh->sh_size, be) / SWAP32(symtab_sh->sh_entsize, be);
	strtab = map + SWAP32(shdr[SWAP32(symtab_sh->sh_link, be)].sh_offset, be);

	for (i = 0; i < num_syms; i++)
	{
		int type = ELF32_ST_TYPE(syms[i].st_info);
		uint32_t name = SWAP32(syms[i].st_name, be);
		char c;

		if (!name || type == STT_FILE || type == STT_SECTION)
			continue;
		c = get_type_32(&syms[i], shdr, shnum, be);
		if (c == 'U' || c == 'w' || c == 'v')
			printf("         %c %s\n", c, strtab + name);
		else
			printf("%08x %c %s\n", SWAP32(syms[i].st_value, be), c, strtab + name);
	}
	return (0);
}
