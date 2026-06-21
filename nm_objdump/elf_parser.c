#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <elf.h>

#include "elf_parser.h"

static char get_symbol_type(Elf64_Sym *sym, Elf64_Shdr *shdrs)
{
    unsigned char bind = ELF64_ST_BIND(sym->st_info);
    unsigned char type = ELF64_ST_TYPE(sym->st_info);

    if (sym->st_shndx == SHN_UNDEF)
        return 'U';
    if (sym->st_shndx == SHN_ABS)
        return 'A';
    if (sym->st_shndx == SHN_COMMON)
        return 'C';

    if (type == STT_OBJECT)
    {
        if (shdrs[sym->st_shndx].sh_type == SHT_NOBITS)
            return 'B';
        return 'D';
    }

    if (type == STT_FUNC)
        return 'T';

    if (bind == STB_WEAK)
    {
        if (sym->st_shndx == SHN_UNDEF)
            return 'w';
        return 'W';
    }

    return 'T';
}

static void print_symbol(Elf64_Sym *sym, char *strtab, char type)
{
    if (type == 'U')
        printf("%17s %c %s\n", "", type, strtab + sym->st_name);
    else
        printf("%016lx %c %s\n",
            (unsigned long)sym->st_value,
            type,
            strtab + sym->st_name);
}

int parse_elf(const char *filename)
{
    int fd;
    Elf64_Ehdr ehdr;
    Elf64_Shdr *shdrs;
    Elf64_Shdr shstr;
    Elf64_Sym *syms;
    char *shstrtab;
    char *strtab;
    int i, j, symcount;
    int found = 0;

    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror(filename);
        return (1);
    }

    if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr))
        return (close(fd), 1);

    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0)
        return (close(fd), 1);

    if (ehdr.e_shoff == 0 || ehdr.e_shnum == 0)
        return (close(fd), 1);

    shdrs = malloc(ehdr.e_shentsize * ehdr.e_shnum);
    if (!shdrs)
        return (close(fd), 1);

    lseek(fd, ehdr.e_shoff, SEEK_SET);
    read(fd, shdrs, ehdr.e_shentsize * ehdr.e_shnum);

    shstr = shdrs[ehdr.e_shstrndx];

    shstrtab = malloc(shstr.sh_size);
    if (!shstrtab)
        return (free(shdrs), close(fd), 1);

    lseek(fd, shstr.sh_offset, SEEK_SET);
    read(fd, shstrtab, shstr.sh_size);

    for (i = 0; i < ehdr.e_shnum; i++)
    {
       if (shdrs[i].sh_type == SHT_SYMTAB ||
    shdrs[i].sh_type == SHT_DYNSYM)
            continue;

        found = 1;

        syms = malloc(shdrs[i].sh_size);
        lseek(fd, shdrs[i].sh_offset, SEEK_SET);
        read(fd, syms, shdrs[i].sh_size);

        strtab = malloc(shdrs[shdrs[i].sh_link].sh_size);
        lseek(fd, shdrs[shdrs[i].sh_link].sh_offset, SEEK_SET);
        read(fd, strtab, shdrs[shdrs[i].sh_link].sh_size);

        symcount = shdrs[i].sh_size / sizeof(Elf64_Sym);

        for (j = 0; j < symcount; j++)
        {
            if (syms[j].st_name)
            {
                char type = get_symbol_type(&syms[j], shdrs);
                print_symbol(&syms[j], strtab, type);
            }
        }

        free(strtab);
        free(syms);
    }

    if (!found)
        fprintf(stderr, "%s: no symbols\n", filename);

    free(shstrtab);
    free(shdrs);
    close(fd);

    return (0);
}
