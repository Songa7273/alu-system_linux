#include "hnm.h"

/**
 * parse_elf - Validates and maps ELF file targets
 * @filename: Name of target file
 * Return: 0 on success, 1 on failure
 */
int parse_elf(const char *filename)
{
	int fd, status = 0;
	struct stat st;
	char *map;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
		return (1);
	if (fstat(fd, &st) < 0 || st.st_size < (long)sizeof(Elf32_Ehdr))
	{
		close(fd);
		return (1);
	}
	map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (map == MAP_FAILED)
	{
		close(fd);
		return (1);
	}
	if (map[EI_MAG0] != ELFMAG0 || map[EI_MAG1] != ELFMAG1 ||
		map[EI_MAG2] != ELFMAG2 || map[EI_MAG3] != ELFMAG3)
	{
		munmap(map, st.st_size);
		close(fd);
		return (1);
	}
	if (map[EI_CLASS] == ELFCLASS64)
		status = process_64(filename, map, (Elf64_Ehdr *)map);
	else if (map[EI_CLASS] == ELFCLASS32)
		status = process_32(filename, map, (Elf32_Ehdr *)map);

	munmap(map, st.st_size);
	close(fd);
	return (status);
}

/**
 * main - Entry point for hnm executable
 * @argc: Argument count
 * @argv: Argument vector
 * Return: 0 on success, or status code from operations
 */
int main(int argc, char **argv)
{
	if (argc < 2)
		return (parse_elf("a.out"));
	return (parse_elf(argv[1]));
}
