CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic

SRC = hnm.c elf_parser.c
OBJ = $(SRC:.c=.o)

all: hnm

hnm: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o hnm

clean:
	rm -f *.o

fclean: clean
	rm -f hnm

re: fclean all

.PHONY: all clean fclean re
