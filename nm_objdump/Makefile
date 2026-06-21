CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -std=gnu89

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
NAME = hnm

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
