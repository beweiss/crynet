#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <error.h>
#include <arpa/inet.h>
#include <string.h>
#include "crynet.h"

int start_server()
{
	int server;
	struct sockaddr_in server_addr;

	if ((server = socket(AF_INET, SOCK_STREAM, 0)) == -1) error(1, errno, "socket");

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(6789);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(server, (struct sockaddr *) &server_addr, (socklen_t) sizeof(server_addr)) == -1) error(1, errno, "bind");

	if (listen(server, 10) == -1) error(1, errno, "listen");

	return server;
}

int accept_client(int server)
{
	int client;

	if ((client = accept(server, NULL, NULL)) == -1)
	{
		errlog(SYS_FAIL, "accept_client: accept");
	}
	return client;
}

struct user_data *fill_user_data(int client)
{
	struct user_data *ret;
	unsigned char *buffer;
	unsigned char key;
	uint32_t size;

	if (read(client, &key, sizeof(char)) == -1) //1. key
	{
		errlog(FAIL_SYS, "fill_user_data: read (key)");
		return NULL;
	}
	if (read(client, &size, sizeof(uint32_t)) == -1) //2. size
	{
		errlog(FAIL_SYS, "fill_user_data: read (size)");
		return NULL;
	}
	if (!(size = ntohl(size))) //if size == 0 then return NULL
	{
		errlog(0, "fill_user_data: (size == 0)");
		return NULL;
	}
	if (!(buffer = malloc(size))) //malloc for user name and password
	{
		errlog(SYS_FAIL, "fill_user_data: malloc (buffer)");
		return NULL;
	}
	if (read(client, buffer, size * sizeof(char)) == -1) //3. read user name and password
	{
		errlog(FAIL_SYS, "fill_user_data: read (userdata)");
		return NULL;
	}
	if (!(buffer = decrypt_text(size, buffer, key))) //decrypt user name and password
	{
		errlog(0, "fill_user_data: decrypt_text");
		return NULL;
	}
	if (!(ret = malloc(sizeof(struct user_data))))
	{
		errlog(SYS_FAIL, "fill_user_data: malloc (ret)");
		return NULL;
	}
	if (!(ret->data = malloc(size + 1)))
	{
		errlog(SYS_FAIL, "fill_user_data: malloc (ret->data)");
		return NULL;
	}
	ret->client = client;
	memcpy(ret->data, buffer, size);
	ret->data[size] = 0;
	free(buffer);
	return ret;
}

void del_user_data(struct user_data *arg)
{
	free(arg->data);
	free(arg);
	return;
}
