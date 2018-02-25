#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <error.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "crynet.h"

int connect_with_server()
{
	int sock;
	struct sockaddr_in server;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) error(1, errno, "socket");

	server.sin_family = AF_INET;
	server.sin_port = htons(6789);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (connect(sock, (struct sockaddr *) &server, (socklen_t) sizeof(server)) == -1) error(1, errno, "connect");
	return sock;
}

void write_data(int socket, struct packet_data *packet)
{
	unsigned char *buffer;

	if (!(buffer = malloc(sizeof(packet->header) + sizeof(packet->size) + packet->size)))
	{
		error(1, errno, "malloc(3)");
	}
	*buffer = packet->header;
	buffer++;
	((uint32_t *) buffer)[0] = htonl(packet->size);
	memcpy(buffer + sizeof(packet->size), packet->data, packet->size);
	buffer--;
	if (write(socket, buffer, (sizeof(packet->header) + sizeof(packet->size) + packet->size)) == -1)
	{
		fprintf(stderr, "ERROR: Couldn't write data (packet) to server\n");
	}
	free(buffer);
	return;
}

unsigned char *read_data(int socket)
{
	unsigned char *buffer = NULL;
	unsigned char *last = NULL;
	unsigned int i = 0;
	unsigned int j = 1;
	int proof;

	do
	{
		if (!(i & 255))
		{
			last = buffer;
			buffer = realloc(buffer, 256 * j++);
			if (buffer == NULL)
			{
				free(last);
				return (unsigned char*) "ERROR: Failed to execute realloc(3)";
			}
		}
		proof = read(socket, buffer + i, 1);
		if (proof == -1)
		{
			free(buffer);
			return (unsigned char*) "ERROR: Couldn't read from socket";
		}
		i++;

	} while (proof != 0);
	return buffer;
}
