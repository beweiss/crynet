#define PRG "crypto"

#define syserr_fatal(expr, func) ({if ((expr)) {perror((PRG": "func)); exit(1);}})

struct crypt_data
{
	unsigned char key;
	unsigned char *code;
	unsigned int size;
};

struct packet_data
{
	unsigned char header;
	unsigned char *data;
	unsigned int size;
};

/* network.c */
int connect_with_server();
void write_data(int socket, struct packet_data *packet);
unsigned char *read_data(int socket);
unsigned int string_length(unsigned char *string);
unsigned char *string_n_copy(unsigned char *dest, unsigned char *src, unsigned int n);

/* crypto.c */
struct crypt_data *encrypto(unsigned char *text, unsigned int size);
char *decrypto(struct crypt_data *data);
