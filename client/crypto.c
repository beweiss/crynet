#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "crynet.h"

struct crypt_data *encrypto(unsigned char *text, unsigned int size)
{
	struct crypt_data *cryptoc;
	unsigned int i = 0;
	int fd = open("/dev/urandom", O_RDONLY);

	syserr_fatal(fd == -1, "open(2)");

	if (!(cryptoc = malloc(sizeof(struct crypt_data)))) error(1, errno, "malloc(3)");

	syserr_fatal(read(fd, &(cryptoc->key), sizeof(char)) == -1, "read(2)");
	syserr_fatal(close(fd) == -1, "close(2)");

	cryptoc->code = malloc(size * sizeof(char));

	for (i = 0; i < size; i++)
	{
		cryptoc->code[i] = text[i] ^ cryptoc->key;
	}
	cryptoc->size = size;
	return cryptoc;
}

char *decrypto(struct crypt_data *data)
{
	unsigned int i = 0;
	unsigned int len = data->size;
	unsigned char key = data->key;
	unsigned char *pointer = data->code;
	char *plain = malloc((len + 1) * sizeof(char));

	while (i != len)
	{
		plain[i] = *pointer++ ^ key;
		i++;
	}
	plain[i] = 0;
	return plain;
}
