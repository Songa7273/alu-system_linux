#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <elf.h>

#include "elf_parser.h"

static void print_symbol_64(Elf64_Sym *sym, char *strtab)
{
    char type;

    if (ELF64_ST_BIND(sym->st_info) == STB_WEAK)
        type = 'W';
    else if (sym->st_shndx == SHN_UNDEF)
        type = 'U';
    else
        type = 'T';

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
    Elf64_Shdr shdr;
    Elf64_Sym *syms;
    char *shstrtab;
    char *strtab;
    int i;
    int j;
    int symcount;

    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        perror(filename);
        return (1);
    }

    if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr))
    {
        close(fd);
        return (1);
    }

    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0)
    {
        fprintf(stderr, "%s: not an ELF file\n", filename);
        close(fd);
        return (1);
    }

    shdrs = malloc(ehdr.e_shentsize * ehdr.e_shnum);
    if (!shdrs)
        return (1);

    lseek(fd, ehdr.e_shoff, SEEK_SET);
    read(fd, shdrs, ehdr.e_shentsize * ehdr.e_shnum);

    shdr = shdrs[ehdr.e_shstrndx];

    shstrtab = malloc(shdr.sh_size);
    if (!shstrtab)
        return (1);

    lseek(fd, shdr.sh_offset, SEEK_SET);
    read(fd, shstrtab, shdr.sh_size);

    for (i = 0; i < ehdr.e_shnum; i++)
    {
        if (shdrs[i].sh_type == SHT_SYMTAB)
        {
            syms = malloc(shdrs[i].sh_size);
            if (!syms)
                return (1);

            lseek(fd, shdrs[i].sh_offset, SEEK_SET);
            read(fd, syms, shdrs[i].sh_size);

            strtab = malloc(shdrs[shdrs[i].sh_link].sh_size);
            if (!strtab)
                return (1);

            lseek(fd, shdrs[shdrs[i].sh_link].sh_offset, SEEK_SET);
            read(fd, strtab, shdrs[shdrs[i].sh_link].sh_size);

            symcount = shdrs[i].sh_size / sizeof(Elf64_Sym);

            for (j = 0; j < symcount; j++)
            {
                if (syms[j].st_name)
                    print_symbol_64(&syms[j], strtab);
            }

            free(strtab);
            free(syms);
        }
    }

    free(shstrtab);
    free(shdrs);
    close(fd);

    return (0);
}
