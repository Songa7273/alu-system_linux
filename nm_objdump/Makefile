CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -std=gnu99

# Binaries
NM = hnm
OBJDUMP = hobjdump

# Source files
NM_SRC = hnm.c main.c
OBJ_SRC = hobjdump.c hobjdump_32.c main_objdump.c

# Object files
NM_OBJ = $(NM_SRC:.c=.o)
OBJ_OBJ = $(OBJ_SRC:.c=.o)

.PHONY: all clean oclean fclean re

all: $(NM) $(OBJDUMP)

$(NM): $(NM_OBJ)
	$(CC) $(CFLAGS) $(NM_OBJ) -o $(NM)

$(OBJDUMP): $(OBJ_OBJ)
	$(CC) $(CFLAGS) $(OBJ_OBJ) -o $(OBJDUMP)

%.o: %.c hobjdump.h
	$(CC) $(CFLAGS) -c $< -o $@

clean: oclean fclean

oclean:
	rm -f $(NM_OBJ) $(OBJ_OBJ)

fclean:
	rm -f $(NM) $(OBJDUMP)

re: oclean fclean all
