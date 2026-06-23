#include "hnm.h"

/**
 * get_type_64 - Determines the nm char type for a 64-bit ELF symbol
 * @sym: Pointer to Elf64_Sym
 * @shdr: Pointer to section headers
 * @shnum: Number of sections
 * Return: Character flag representing the type
 */
char get_type_64(Elf64_Sym *sym, Elf64_Shdr *shdr, int shnum)
{
	char c = '?';

	if (ELF64_ST_BIND(sym->st_info) == STB_WEAK)
	{
		c = (sym->st_shndx == SHN_UNDEF) ? 'w' : 'W';
		if (ELF64_ST_TYPE(sym->st_info) == STT_OBJECT)
			c = (sym->st_shndx == SHN_UNDEF) ? 'v' : 'V';
		return (c);
	}
	if (sym->st_shndx == SHN_UNDEF)
		return ('U');
	if (sym->st_shndx == SHN_ABS)
		c = 'A';
	else if (sym->st_shndx == SHN_COMMON)
		c = 'C';
	else if (sym->st_shndx < shnum)
	{
		uint32_t type = shdr[sym->st_shndx].sh_type;
		uint64_t flags = shdr[sym->st_shndx].sh_flags;

		if (type == SHT_NOBITS)
			c = 'B';
		else if (flags & SHF_WRITE)
			c = 'D';
		else if (flags & SHF_EXECINSTR)
			c = 'T';
		else
			c = 'R';
	}
	if (ELF64_ST_BIND(sym->st_info) == STB_LOCAL && c != '?')
		c += 32; /* Convert to lowercase for local symbols */
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
	Elf64_Shdr *shdr = (Elf64_Shdr *)(map + ehdr->e_shoff);
	Elf64_Shdr *symtab_sh = NULL;
	Elf64_Sym *syms;
	char *strtab;
	int i, shnum = ehdr->e_shnum, num_syms;

	for (i = 0; i < shnum; i++)
	{
		if (shdr[i].sh_type == SHT_SYMTAB)
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
	syms = (Elf64_Sym *)(map + symtab_sh->sh_offset);
	num_syms = symtab_sh->sh_size / symtab_sh->sh_entsize;
	strtab = map + shdr[symtab_sh->sh_link].sh_offset;

	for (i = 0; i < num_syms; i++)
	{
		int type = ELF64_ST_TYPE(syms[i].st_info);
		char c;

		if (!syms[i].st_name || type == STT_FILE || type == STT_SECTION)
			continue;
		c = get_type_64(&syms[i], shdr, shnum);
		if (c == 'U' || c == 'w' || c == 'v')
			printf("                 %c %s\n", c, strtab + syms[i].st_name);
		else
			printf("%016lx %c %s\n", syms[i].st_value, c, strtab + syms[i].st_name);
	}
	return (0);
}

/**
 * get_type_32 - Determines the nm char type for a 32-bit ELF symbol
 * @sym: Pointer to Elf32_Sym
 * @shdr: Pointer to section headers
 * @shnum: Number of sections
 * Return: Character flag representing the type
 */
char get_type_32(Elf32_Sym *sym, Elf32_Shdr *shdr, int shnum)
{
	char c = '?';

	if (ELF32_ST_BIND(sym->st_info) == STB_WEAK)
	{
		c = (sym->st_shndx == SHN_UNDEF) ? 'w' : 'W';
		if (ELF32_ST_TYPE(sym->st_info) == STT_OBJECT)
			c = (sym->st_shndx == SHN_UNDEF) ? 'v' : 'V';
		return (c);
	}
	if (sym->st_shndx == SHN_UNDEF)
		return ('U');
	if (sym->st_shndx == SHN_ABS)
		c = 'A';
	else if (sym->st_shndx == SHN_COMMON)
		c = 'C';
	else if (sym->st_shndx < shnum)
	{
		uint32_t type = shdr[sym->st_shndx].sh_type;
		uint32_t flags = shdr[sym->st_shndx].sh_flags;

		if (type == SHT_NOBITS)
			c = 'B';
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
	Elf32_Shdr *shdr = (Elf32_Shdr *)(map + ehdr->e_shoff);
	Elf32_Shdr *symtab_sh = NULL;
	Elf32_Sym *syms;
	char *strtab;
	int i, shnum = ehdr->e_shnum, num_syms;

	for (i = 0; i < shnum; i++)
	{
		if (shdr[i].sh_type == SHT_SYMTAB)
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
	syms = (Elf32_Sym *)(map + symtab_sh->sh_offset);
	num_syms = symtab_sh->sh_size / symtab_sh->sh_entsize;
	strtab = map + shdr[symtab_sh->sh_link].sh_offset;

	for (i = 0; i < num_syms; i++)
	{
		int type = ELF32_ST_TYPE(syms[i].st_info);
		char c;

		if (!syms[i].st_name || type == STT_FILE || type == STT_SECTION)
			continue;
		c = get_type_32(&syms[i], shdr, shnum);
		if (c == 'U' || c == 'w' || c == 'v')
			printf("         %c %s\n", c, strtab + syms[i].st_name);
		else
			printf("%08x %c %s\n", syms[i].st_value, c, strtab + syms[i].st_name);
	}
	return (0);
}
