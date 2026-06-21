#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <elf.h>
#include "elf_parser.h"

static char get_symbol_type64(Elf64_Sym *sym, Elf64_Shdr *shdrs);
static void print_symbol64(Elf64_Sym *sym, char *strtab, char type);
static int parse_elf64(int fd, Elf64_Ehdr *ehdr);
static char get_symbol_type32(Elf32_Sym *sym, Elf32_Shdr *shdrs);
static void print_symbol32(Elf32_Sym *sym, char *strtab, char type);
static int parse_elf32(int fd, Elf32_Ehdr *ehdr);

/**
 * get_symbol_type64 - Gets the nm type character for a 64-bit symbol
 * @sym: Pointer to symbol structure
 * @shdrs: Pointer to section headers array
 * Return: Type character
 */
static char get_symbol_type64(Elf64_Sym *sym, Elf64_Shdr *shdrs)
{
	unsigned char bind = ELF64_ST_BIND(sym->st_info);
	unsigned char type = ELF64_ST_TYPE(sym->st_info);

	if (sym->st_shndx == SHN_UNDEF)
		return (bind == STB_WEAK ? 'w' : 'U');
	if (sym->st_shndx == SHN_ABS)
		return (bind == STB_WEAK ? 'w' : 'A');
	if (sym->st_shndx == SHN_COMMON)
		return ('C');
	if (bind == STB_WEAK)
		return (type == STT_OBJECT ? 'V' : 'W');
	if (shdrs[sym->st_shndx].sh_type == SHT_NOBITS)
		return (bind == STB_LOCAL ? 'b' : 'B');
	if (shdrs[sym->st_shndx].sh_type == SHT_PROGBITS)
	{
		if (shdrs[sym->st_shndx].sh_flags & SHF_EXECINSTR)
			return (bind == STB_LOCAL ? 't' : 'T');
		if (shdrs[sym->st_shndx].sh_flags & SHF_WRITE)
			return (bind == STB_LOCAL ? 'd' : 'D');
		return (bind == STB_LOCAL ? 'r' : 'R');
	}
	return (bind == STB_LOCAL ? 't' : 'T');
}

/**
 * print_symbol64 - Prints a single 64-bit symbol entry
 * @sym: Pointer to symbol
 * @strtab: Pointer to string table string data
 * @type: Symbol type character flag
 */
static void print_symbol64(Elf64_Sym *sym, char *strtab, char type)
{
	if (type == 'U' || type == 'w')
		printf("%16s %c %s\n", "", type, strtab + sym->st_name);
	else
		printf("%016lx %c %s\n",
			(unsigned long)sym->st_value, type, strtab + sym->st_name);
}

/**
 * parse_elf64 - Parsers a 64-bit ELF workspace
 * @fd: File descriptor
 * @ehdr: Pointer to ELF header structure
 * Return: 0 on success, 1 on error
 */
static int parse_elf64(int fd, Elf64_Ehdr *ehdr)
{
	Elf64_Shdr *shdrs;
	Elf64_Sym *syms;
	char *strtab;
	int i, j, symcount, found = 0;

	if (ehdr->e_shoff == 0 || ehdr->e_shnum == 0)
		return (1);
	shdrs = malloc(ehdr->e_shentsize * ehdr->e_shnum);
	if (!shdrs)
		return (1);
	lseek(fd, ehdr->e_shoff, SEEK_SET);
	if (read(fd, shdrs, ehdr->e_shentsize * ehdr->e_shnum) <= 0)
		return (free(shdrs), 1);
	for (i = 0; i < ehdr->e_shnum; i++)
	{
		if (shdrs[i].sh_type != SHT_SYMTAB && shdrs[i].sh_type != SHT_DYNSYM)
			continue;
		syms = malloc(shdrs[i].sh_size);
		if (!syms)
			continue;
		lseek(fd, shdrs[i].sh_offset, SEEK_SET);
		if (read(fd, syms, shdrs[i].sh_size) <= 0)
		{
			free(syms);
			continue;
		}
		strtab = malloc(shdrs[shdrs[i].sh_link].sh_size);
		if (!strtab)
		{
			free(syms);
			continue;
		}
		lseek(fd, shdrs[shdrs[i].sh_link].sh_offset, SEEK_SET);
		if (read(fd, strtab, shdrs[shdrs[i].sh_link].sh_size) <= 0)
		{
			free(strtab);
			free(syms);
			continue;
		}
		symcount = shdrs[i].sh_size / sizeof(Elf64_Sym);
		for (j = 0; j < symcount; j++)
		{
			if (syms[j].st_name && ELF64_ST_TYPE(syms[j].st_info) != STT_SECTION)
			{
				char type = get_symbol_type64(&syms[j], shdrs);

				print_symbol64(&syms[j], strtab, type);
				found = 1;
			}
		}
		free(strtab);
		free(syms);
	}
	free(shdrs);
	return (found ? 0 : -1);
}

/**
 * get_symbol_type32 - Gets the nm type character for a 32-bit symbol
 * @sym: Pointer to symbol structure
 * @shdrs: Pointer to section headers array
 * Return: Type character
 */
static char get_symbol_type32(Elf32_Sym *sym, Elf32_Shdr *shdrs)
{
	unsigned char bind = ELF32_ST_BIND(sym->st_info);
	unsigned char type = ELF32_ST_TYPE(sym->st_info);

	if (sym->st_shndx == SHN_UNDEF)
		return (bind == STB_WEAK ? 'w' : 'U');
	if (sym->st_shndx == SHN_ABS)
		return (bind == STB_WEAK ? 'w' : 'A');
	if (sym->st_shndx == SHN_COMMON)
		return ('C');
	if (bind == STB_WEAK)
		return (type == STT_OBJECT ? 'V' : 'W');
	if (shdrs[sym->st_shndx].sh_type == SHT_NOBITS)
		return (bind == STB_LOCAL ? 'b' : 'B');
	if (shdrs[sym->st_shndx].sh_type == SHT_PROGBITS)
	{
		if (shdrs[sym->st_shndx].sh_flags & SHF_EXECINSTR)
			return (bind == STB_LOCAL ? 't' : 'T');
		if (shdrs[sym->st_shndx].sh_flags & SHF_WRITE)
			return (bind == STB_LOCAL ? 'd' : 'D');
		return (bind == STB_LOCAL ? 'r' : 'R');
	}
	return (bind == STB_LOCAL ? 't' : 'T');
}

/**
 * print_symbol32 - Prints a single 32-bit symbol entry
 * @sym: Pointer to symbol
 * @strtab: Pointer to string table string data
 * @type: Symbol type character flag
 */
static void print_symbol32(Elf32_Sym *sym, char *strtab, char type)
{
	if (type == 'U' || type == 'w')
		printf("%8s %c %s\n", "", type, strtab + sym->st_name);
	else
		printf("%08lx %c %s\n",
			(unsigned long)sym->st_value, type, strtab + sym->st_name);
}

/**
 * parse_elf32 - Parsers a 32-bit ELF workspace
 * @fd: File descriptor
 * @ehdr: Pointer to 32-bit ELF header structure
 * Return: 0 on success, 1 on error
 */
static int parse_elf32(int fd, Elf32_Ehdr *ehdr)
{
	Elf32_Shdr *shdrs;
	Elf32_Sym *syms;
	char *strtab;
	int i, j, symcount, found = 0;

	if (ehdr->e_shoff == 0 || ehdr->e_shnum == 0)
		return (1);
	shdrs = malloc(ehdr->e_shentsize * ehdr->e_shnum);
	if (!shdrs)
		return (1);
	lseek(fd, ehdr->e_shoff, SEEK_SET);
	if (read(fd, shdrs, ehdr->e_shentsize * ehdr->e_shnum) <= 0)
		return (free(shdrs), 1);
	for (i = 0; i < ehdr->e_shnum; i++)
	{
		if (shdrs[i].sh_type != SHT_SYMTAB && shdrs[i].sh_type != SHT_DYNSYM)
			continue;
		syms = malloc(shdrs[i].sh_size);
		if (!syms)
			continue;
		lseek(fd, shdrs[i].sh_offset, SEEK_SET);
		if (read(fd, syms, shdrs[i].sh_size) <= 0)
		{
			free(syms);
			continue;
		}
		strtab = malloc(shdrs[shdrs[i].sh_link].sh_size);
		if (!strtab)
		{
			free(syms);
			continue;
		}
		lseek(fd, shdrs[shdrs[i].sh_link].sh_offset, SEEK_SET);
		if (read(fd, strtab, shdrs[shdrs[i].sh_link].sh_size) <= 0)
		{
			free(strtab);
			free(syms);
			continue;
		}
		symcount = shdrs[i].sh_size / sizeof(Elf32_Sym);
		for (j = 0; j < symcount; j++)
		{
			if (syms[j].st_name && ELF32_ST_TYPE(syms[j].st_info) != STT_SECTION)
			{
				char type = get_symbol_type32(&syms[j], shdrs);

				print_symbol32(&syms[j], strtab, type);
				found = 1;
			}
		}
		free(strtab);
		free(syms);
	}
	free(shdrs);
	return (found ? 0 : -1);
}

/**
 * parse_elf - Dispatches parsing of ELF file to 32 or 64 bit drivers
 * @filename: Targeted filepath string
 * Return: 0 on success, 1 on compilation breakdown
 */
int parse_elf(const char *filename)
{
	int fd, res = 0;
	unsigned char ident[EI_NIDENT];

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		perror(filename);
		return (1);
	}
	if (read(fd, ident, EI_NIDENT) != EI_NIDENT)
	{
		close(fd);
		return (1);
	}
	if (memcmp(ident, ELFMAG, SELFMAG) != 0)
	{
		close(fd);
		return (1);
	}
	lseek(fd, 0, SEEK_SET);
	if (ident[EI_CLASS] == ELFCLASS64)
	{
		Elf64_Ehdr ehdr;

		if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr))
			return (close(fd), 1);
		res = parse_elf64(fd, &ehdr);
	}
	else if (ident[EI_CLASS] == ELFCLASS32)
	{
		Elf32_Ehdr ehdr;

		if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr))
			return (close(fd), 1);
		res = parse_elf32(fd, &ehdr);
	}
	if (res == -1)
		fprintf(stderr, "%s: no symbols\n", filename);
	close(fd);
	return (res == 0 ? 0 : 1);
}
