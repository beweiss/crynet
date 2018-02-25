#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include "crynet.h"
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	struct crypt_data *pointer;
	//struct packet_data *packet;
	unsigned char *code = NULL;
	unsigned char *buffer = NULL;
	int server;

	/*memcpy(code, "Benedikt;1234", 13);
	buffer = malloc(18 * sizeof(char));
	pointer = encrypto(code, 13);
	buffer[0] = pointer->key;
	buffer++;
	((unsigned int *) buffer)[0] = htonl(13);
	buffer--;
	memcpy(buffer + 5, pointer->code, 13);

	server = connect_with_server();
	write(server, buffer, 18);
	close(server);

	free(pointer);
	free(buffer);
	free(code);*/

	code = malloc(12);
	memcpy(code, "Tobias;Linux", 12);
	buffer = malloc(17 * sizeof(char));
	pointer = encrypto(code, 12);
	buffer[0] = pointer->key;
	buffer++;
	((unsigned int *) buffer)[0] = htonl(12);
	buffer--;
	memcpy(buffer + 5, pointer->code, 12);

	server = connect_with_server();
	write(server, buffer, 17);
	close(server);


	/*for (i = 0; i < pointer->size; i++) printf("%u == %u\n", code[i + 1], pointer->code[i]);
	pointer->code = code + 1;
	printf("%s\n", decrypto(pointer));*/

	free(pointer);
	//free(packet);
	free(buffer);
	free(code);

	//write(1, code, pointer->size); //hexdump -C
	exit(0);
}
