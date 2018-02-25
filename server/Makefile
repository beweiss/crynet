CC=gcc -Wall -g
INC=-I./

OBJ= \
network.o \
main.o \
packet.o \
file_sys.o \
login.o \
crypto.o

NAME=server

server: $(OBJ)
	$(CC) $(INC) -o $(NAME) $(OBJ)

%.o: %.c
	$(CC) $(INC) -c -o $@ $<

clean:
	rm $(NAME) $(OBJ)
