#include "hobjdump.h"

/**
 * parse_objfile - Sets up system mappings to isolate target boundaries
 * @filename: Operational file pathway string
 * Return: 0 on standard operations, 1 on failures
 */
int parse_objfile(const char *filename)
{
	int fd, status = 0;
	struct stat st;
	char *map;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "./hobjdump: '%s': No such file\n", filename);
		return (1);
	}
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
		status = process_obj_64(filename, map, (Elf64_Ehdr *)map);
	else if (map[EI_CLASS] == ELFCLASS32)
		status = process_obj_32(filename, map, (Elf32_Ehdr *)map);

	munmap(map, st.st_size);
	close(fd);
	return (status);
}

/**
 * main - Unified program wrapper entry point
 * @argc: Structural arguments total counter
 * @argv: Structural tracking argument references array
 * Return: Final integer status validation flag
 */
int main(int argc, char **argv)
{
	int i, global_status = 0;

	if (argc < 2)
		return (parse_objfile("a.out"));

	for (i = 1; i < argc; i++)
	{
		if (parse_objfile(argv[i]) != 0)
			global_status = 1;
	}
	return (global_status);
}
