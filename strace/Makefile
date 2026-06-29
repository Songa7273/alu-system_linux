CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic

SRC = strace_0.c
OBJ = $(SRC:.c=.o)

all: strace_0

strace_0: $(OBJ)
	$(CC) $(CFLAGS) -o strace_0 $(OBJ)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f strace_0

re: fclean all

.PHONY: all clean fclean re
