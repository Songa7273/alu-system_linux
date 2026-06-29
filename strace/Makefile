CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic

SRC = strace_0.c
OBJ = $(SRC:.c=.o)
NAME = strace_0

.PHONY: all clean fclean re strace_0

all: $(NAME)

# Link the object files to create the final executable
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all
