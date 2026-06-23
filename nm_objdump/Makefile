CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -std=gnu99

# Shared tracking components
NM_SRC = hnm.c main.c
NM_OBJ = $(NM_SRC:.c=.o)

OBJDUMP_SRC = hobjdump.c main_objdump.c
OBJDUMP_OBJ = $(OBJDUMP_SRC:.c=.o)

.PHONY: all clean fclean re hnm hobjdump

all: hnm hobjdump

hnm: $(NM_OBJ)
	$(CC) $(CFLAGS) $(NM_OBJ) -o hnm

hobjdump: $(OBJDUMP_OBJ)
	$(CC) $(CFLAGS) $(OBJDUMP_OBJ) -o hobjdump

clean:
	rm -f $(NM_OBJ) $(OBJDUMP_OBJ)

fclean: clean
	rm -f hnm hobjdump

re: fclean all
