SRC = sha3_main.c , sha3_functions.c

OBJ = $(SRC:.c=.o)

NAME = SHA3

## Functions

all: $(NAME)

$(NAME): $(OBJ)
	gcc $(OBJ) -o $(SRC)

clean:
	rm -r $(OBJ)

fclean: clean
	rm $(NAME)

re: fclean all