CC=gcc -Wall -g
INC=-I./

OBJ= \
crypto.o \
main.o \
network.o

NAME=client

client: $(OBJ)
	$(CC) $(INC) -o $(NAME) $(OBJ)

%.o: %.c
	$(CC) $(INC) -c -o $@ $<

clean:
	rm $(NAME) $(OBJ)
