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

	if (fd == -1)
	{
		errlog(FAIL_SYS, "encrypto: open");
		return NULL;
	}
	if (!(cryptoc = malloc(sizeof(struct crypt_data))))
	{
		errlog(FAIL_SYS, "encrypto: malloc (cryptoc)");
		return NULL;
	}
	if (read(fd, &(cryptoc->key), sizeof(char)) == -1)
	{
		errlog(FAIL_SYS, "encrypto: read");
		return NULL;
	}
	close(fd);
	if (!(cryptoc->code = malloc(size * sizeof(char))))
	{
		errlog(FAIL_SYS, "encrypto: malloc (cryptoc->code)");
		return NULL;
	}
	for (i = 0; i < size; i++)
	{
		cryptoc->code[i] = text[i] ^ cryptoc->key;
	}
	cryptoc->size = size;
	return cryptoc;
}

unsigned char *decrypto(struct crypt_data *data)
{
	unsigned int i = 0;
	unsigned int len = data->size;
	unsigned char key = data->key;
	unsigned char *pointer = data->code;
	unsigned char *plain = malloc(len * sizeof(char));

	while (i < len)
	{
		plain[i] = *pointer++ ^ key;
		i++;
	}
	return plain;
}

unsigned char *decrypt_text(unsigned int size, unsigned char *buffer, unsigned char key)
{
	struct crypt_data *crypt;
	unsigned char *plain;

	if (!(crypt = fill_crypto_struct(size, buffer, key)))
	{
		errlog(FAIL_SYS, "decrypt_text: fill_crypto_struct");
		return NULL;
	}
	plain = decrypto(crypt);
	free(crypt);
	return plain;
}

struct crypt_data *fill_crypto_struct(unsigned int size, unsigned char *buffer, unsigned char key)
{
	struct crypt_data *ret;

	if (!(ret = malloc(sizeof(struct crypt_data))))
	{
		errlog(FAIL_SYS, "fill_crypto_struct: malloc (ret)");
		return NULL;
	}
	if (!(ret->code = malloc(size * sizeof(char))))
	{
		errlog(FAIL_SYS, "fill_crypto_struct: malloc (ret->code)");
		return NULL;
	}
	ret->size = size;
	memcpy(ret->code, buffer, size);
	ret->key = key;
	return ret;
}
