#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "crynet.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static unsigned int rows = 0; /*important to navigate in the profiles.txt!
maybe I can find a better solution for the navigation in the profiles.txt
*/

int client_login(struct user_data *arg)
{
	if (verify_user_data(arg->data) == -1) //parser and scanner...
	{
		errlog(0, "client_login: verify_user_data");
		return -1;
	}
	if (check_login_data(arg->data) == -1) //interpreter...
	{
		errlog(0, "client_login: check_login_data");
		return -1;
	}
	return 0;
}

int verify_user_data(unsigned char *login_data)
{
	if (scan_user_data(login_data) == -1)
	{
		errlog(0, "verify_user_data: scan_user_data");
		return -1;
	}
	if (parse_user_data(login_data) == -1)
	{
		errlog(0, "verify_user_data: parse_user_data");
		return -1;
	}
	return 0;
}

int check_login_data(unsigned char *login_data)
{
	if (check_user_name(login_data) == -1)
	{
		errlog(0, "client_login: check_user_name");
		return -1;
	}
	if (check_user_passw(login_data) == -1)
	{
		errlog(0, "client_login: check_user_passw");
		return -1;
	}
	return 0;
}

int scan_user_data(unsigned char *login_data)
{
	static char *valid_signs = ";0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned int len = strlen((char *) login_data);
	unsigned int i;

	for (i = 0; i < len; i++) if (!login_data[i] || !strchr(valid_signs, login_data[i])) return -1;
	return 0;
}

int parse_user_data(unsigned char *login_data)
{
	unsigned int i = 0;
	unsigned int len = strlen((char *) login_data);
	unsigned char ind = 0;

	if (len > MAX_PASSW_LENGTH) return -1;
	while (i < len)
	{
		if (login_data[i] == ';')
		{
			ind++;
			if (i == 0 || i == len - 1) return -1;
		}
		i++;
	}
	if (ind != 1) return -1;
	return 0;
}

int check_user_name(unsigned char *login_data)
{
	unsigned char *usrnme = extract_usrnme(login_data);
	unsigned char comp;
	unsigned char ind;
	unsigned int i = 0;
	int fd = open("./profiles.txt", O_RDONLY);

	if (fd == -1)
	{
		errlog(SYS_FAIL, "check_user_name: open");
		return -1;
	}
	if (!usrnme)
	{
		errlog(0, "check_user_name: extract_usrnme");
		return -1;
	}
	while (1)
	{
		if ((ind = read(fd, &comp, sizeof(char))) == -1)
		{
			errlog(SYS_FAIL, "check_user_name: read");
			return -1;
		}
		if (!ind)
		{
			errlog(0, "check_user_name: (no matching user, EOF)");
			return -1;
		}
		if (comp == ';' && usrnme[i] == '\0')
		{
			errlog(0, "check_user_name: User: %s tries to log in!", usrnme);
			break;
		}
		if (usrnme[i] != comp)
		{
			while (comp != '\n')
			{
				if (read(fd, &comp, sizeof(char)) == -1)
				{
					errlog(FAIL_SYS, "check_user_name: read (rows)");
					return -1;
				}
			}
			rows++;
			i = 0;
			continue;
		}
		i++;
	}
	close(fd);
	free(usrnme);
	return 0;
}

int check_user_passw(unsigned char *login_data)
{
	unsigned char *passw = extract_passw(login_data);
	unsigned char *usrnme = extract_usrnme(login_data);
	unsigned char comp = 0;
	unsigned char ind;
	unsigned int i = 0;
	int fd = open("./profiles.txt", O_RDONLY);

	if (fd == -1)
	{
		errlog(SYS_FAIL, "check_user_passw: open");
		return -1;
	}
	if (!passw || !usrnme)
	{
		errlog(0, "check_user_passw: extract_passw || extract_usrnme");
		return -1;
	}
	while (i < rows)
	{
		if (read(fd, &comp, sizeof(char)) == -1)
		{
			errlog(FAIL_SYS, "check_user_passw: read (rows)");
			return -1;
		}
		if (comp == '\n') i++;
	}
	i = 0;
	while (comp != ';')
	{
		if (read(fd, &comp, sizeof(char)) == -1)
		{
			errlog(FAIL_SYS, "check_user_passw: read (rows)");
			return -1;
		}
	}
	while (1)
	{
		if ((ind = read(fd, &comp, sizeof(char))) == -1)
		{
			errlog(SYS_FAIL, "check_user_passw: read");
			return -1;
		}
		if (comp == '\n' && passw[i] == '\0')
		{
			errlog(0, "check_user_passw: User: %s logs in successfully!", usrnme);
			break;
		}
		if (passw[i] != comp)
		{
			errlog(0, "check_user_passw: User: %s logs in incorrectly!", usrnme);
			return -1;
		}
		i++;
	}
	free(passw);
	free(usrnme);
	close(fd);
	return 0;
}

unsigned char *extract_usrnme(unsigned char *login_data)
{
	unsigned char *usrnme;
	unsigned int i = 0;

	while (login_data[i] != ';') i++;
	if (!(usrnme = malloc((i + 1) * sizeof(char))))
	{
		errlog(SYS_FAIL, "extract_usrnme: malloc");
		return NULL;
	}
	memcpy(usrnme, login_data, i);
	usrnme[i] = 0;
	return usrnme;
}

unsigned char *extract_passw(unsigned char *login_data)
{
	unsigned char *passw;
	unsigned int len = strlen((char *) login_data);
	unsigned int i = 0;

	while (login_data[i] != ';') i++;
	if (!(passw = malloc((len - i) * sizeof(char))))
	{
		errlog(SYS_FAIL, "extract_passw: malloc");
		return NULL;
	}
	memcpy(passw, login_data + i + 1, len - i - 1);
	passw[len - i - 1] = 0;
	return passw;
}

void logout_user(struct user_data *arg)
{
	unsigned char *usrnme = extract_usrnme(arg->data);

	errlog(0, "User: %s logs out!", usrnme);
	free(usrnme);
	return;
}
