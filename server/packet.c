#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "crynet.h"

void process_command(struct user_data *arg)
{
	unsigned char request;

	if (read(arg->client, &request, sizeof(char)) == -1)
	{
		errlog(SYS_FAIL, "process_command: read");
		return;
	}
	switch (request)
	{
		case REQ_READ:	req_handler_read(arg);
				break;

		case REQ_WRITE:	req_handler_write(arg);
				break;

		default:	req_handler_unimplemented();
	}
	return;
}

inline void req_handler_read(struct user_data *arg)
{
	unsigned char *buffer;
	unsigned int size;

	if (!(buffer = read_file(arg, &size)))
	{
		errlog(0, "req_handler_read: read_file");
		return;
	}
	send_packet(arg->client, buffer, ++size); //because of the preceded NUL byte
	free(buffer);
	return;
}

inline void req_handler_write(struct user_data *arg)
{
	uint32_t size;

	if (read(arg->client, &size, sizeof(uint32_t)) == -1)
	{
		errlog(FAIL_SYS, "req_handler_write: read");
		return;
	}
	if (write_file(arg, ntohl(size)) == 0)
	{
		send_packet(arg->client, (unsigned char *) "\0", 1);
	}
	else errlog(0, "req_handler_write: Failure in write_file");
	return;
}

inline void req_handler_unimplemented()
{
	errlog(0, "req_handler_unimplemented: Bad request! (incorrect header)");
	return;
}

inline void send_packet(int socket, unsigned char *buffer, unsigned int size)
{
	if (write(socket, buffer, size * sizeof(char)) == -1) errlog(SYS_FAIL, "send_packet: write");
	return;
}
