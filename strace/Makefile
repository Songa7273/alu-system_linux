CC = gcc
CFLAGS = -g3 -Wall -Werror -Wextra -pedantic

SRC0 = 0-strace.c
SRC1 = 1-strace.c
SRC2 = 2-strace.c
SRC3 = 3-strace.c

OBJ0 = $(SRC0:.c=.o)
OBJ1 = $(SRC1:.c=.o)
OBJ2 = $(SRC2:.c=.o)
OBJ3 = $(SRC3:.c=.o)

strace_0: $(OBJ0)
	$(CC) $(CFLAGS) $(OBJ0) -o strace_0

strace_1: $(OBJ1)
	$(CC) $(CFLAGS) $(OBJ1) -o strace_1

strace_2: $(OBJ2)
	$(CC) $(CFLAGS) $(OBJ2) -o strace_2

strace_3: $(OBJ3)
	$(CC) $(CFLAGS) $(OBJ3) -o strace_3

clean:
	rm -f *.o

fclean: clean
	rm -f strace_0 strace_1 strace_2 strace_3

re: fclean all

all: strace_0 strace_1 strace_2 strace_3

.PHONY: all clean fclean re
