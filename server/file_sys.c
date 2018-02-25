#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include "crynet.h"

unsigned char *read_file(struct user_data *arg, unsigned int *size)
{
	struct flock lock;
	struct stat statbuf;
	unsigned char *buffer;
	unsigned char *usrnme = extract_usrnme(arg->data);
	unsigned int len = strlen((char *) usrnme);
	unsigned char *path = malloc((len + 7) * sizeof(char));
	int fd;

	if (chdir("./profiles") == -1)
	{
		errlog(SYS_FAIL, "read_file: chdir (in)");
		return NULL;
	}
	memcpy(path, "./", 2);
	memcpy(path + 2, usrnme, len);
	memcpy(path + len + 2, ".txt", 4);
	path[len + 6] = 0;
	if (!usrnme)
	{
		errlog(0, "read_file: extract_usrnme");
		return NULL;
	}
	if (access((char *) path, F_OK) == -1)
	{
		errlog(SYS_FAIL, "read_file: access");
		return NULL;
	}
	if ((fd = open((char *) path, O_RDONLY)) == -1)
	{
		errlog(SYS_FAIL, "read_file: open");
		return NULL;
	}
	lock.l_type = F_RDLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	if (fcntl(fd, F_SETLKW, &lock) == -1)
	{
		errlog(SYS_FAIL, "read_file: fcntl");
		return NULL;
	}
	if (fstat(fd, &statbuf) == -1)
	{
		errlog(SYS_FAIL, "read_file: fstat");
		return NULL;
	}
	*size = (unsigned int) statbuf.st_size;
	if (!(buffer = malloc(((*size) + 1) * sizeof(char))))
	{
		errlog(SYS_FAIL, "read_file: malloc");
		return NULL;
	}
	*buffer = 0;
	if (read(fd, (buffer + 1), (*size) * sizeof(char)) == -1)
	{
		errlog(SYS_FAIL, "read_file: read");
		free(buffer);
		return NULL;
	}
	lock.l_type = F_UNLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	if (fcntl(fd, F_SETLK, &lock) == -1)
	{
		errlog(SYS_FAIL, "read_file: fcntl");
		free(buffer);
		return NULL;
	}
	if (chdir("../") == -1)
	{
		errlog(SYS_FAIL, "read_file: chdir (out)");
		return NULL;
	}
	close(fd);
	free(usrnme);
	free(path);
	return buffer;
}

int write_file(struct user_data *arg, unsigned int size)
{
	struct flock lock;
	char *buffer = NULL;
	unsigned char *usrnme = extract_usrnme(arg->data);
	unsigned int len = strlen((char *) usrnme);
	unsigned char *path = malloc((len + 7) * sizeof(char));
	int fd;

	if (chdir("./profiles") == -1)
	{
		errlog(SYS_FAIL, "read_file: chdir (in)");
		return -1;
	}
	memcpy(path, "./", 2);
	memcpy(path + 2, usrnme, len);
	memcpy(path + len + 2, ".txt", 4);
	path[len + 6] = 0;
	if (access((char *) path, F_OK) == -1) fd = open((char *) path, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
	else fd = open((char *) path, O_WRONLY|O_TRUNC);
	if (fd == -1)
	{
		errlog(SYS_FAIL, "write_file: open");
		return -1;
	}
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	if (fcntl(fd, F_SETLKW, &lock) == -1)
	{
		errlog(SYS_FAIL, "write_file: fcntl");
		return -1;
	}
	if (!(buffer = malloc(size * sizeof(char))))
	{
		errlog(SYS_FAIL, "write_file: malloc");
		return -1;
	}
	if (read(arg->client, buffer, size * sizeof(char)) == -1)
	{
		errlog(0, "write_file: Couldn't read from client");
		errlog(SYS_FAIL, "write_file: read");
		free(buffer);
		return -1;
	}
	if (write(fd, buffer, size * sizeof(char)) == -1)
	{
		errlog(0, "write_file: Failed to write data to file");
		errlog(SYS_FAIL, "write_file: write");
		free(buffer);
		return -1;
	}
	lock.l_type = F_UNLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	if (fcntl(fd, F_SETLK, &lock) == -1)
	{
		errlog(0, "write_file: Failed to unlock file");
		errlog(SYS_FAIL, "write_file: fcntl");
		return -1;
	}
	if (chdir("../") == -1)
	{
		errlog(SYS_FAIL, "read_file: chdir (out)");
		return -1;
	}
	close(fd);
	free(buffer);
	free(usrnme);
	free(path);
	return 0;
}
