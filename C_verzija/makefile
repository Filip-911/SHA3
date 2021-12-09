SRC = sha3_main.c, sha3_functions.c

OBJ = $(SRC:.c=.o)

NAME = sha3

## Functions

all: sha3_main
sha3_main : sha3_functions.o sha3_main.o
	gcc –o sha3_main sha3_functions.o sha3_main.o

clean:
	rm -r $(OBJ)