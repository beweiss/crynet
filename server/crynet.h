#define SYS_FAIL	(-1)
#define FAIL_SYS	SYS_FAIL

struct crypt_data
{
	unsigned char key;
	unsigned char *code;
	unsigned int size;
};

struct user_data
{
	int client;
	unsigned char *data;
};

/* main.c */
void init_server();
void logout_user(struct user_data *arg);
void sig_handler(int signum);
#define error_fail(code, fmt, ...)	{ \
						errlog((int) code, fmt, ##__VA_ARGS__); \
						error(1, (int) code == SYS_FAIL ? errno : 0, fmt, ##__VA_ARGS__); \
					}
void errlog(int code, char *fmt, ...);

/* login.c */
#define MAX_PASSW_LENGTH	20
int client_login(struct user_data *arg);
int verify_user_data(unsigned char *login_data);
int check_login_data(unsigned char *login_data);
int scan_user_data(unsigned char *login_data);
int parse_user_data(unsigned char *login_data);
int check_user_name(unsigned char *login_data);
int check_user_passw(unsigned char *login_data);
unsigned char *extract_usrnme(unsigned char *login_data);
unsigned char *extract_passw(unsigned char *login_data);
void logout_user(struct user_data *arg);

/* network.c */
int start_server();
int accept_client(int server);
struct user_data *fill_user_data(int client);
void del_user_data(struct user_data *arg);

/* packet.c */
#define REQ_READ 48
#define REQ_WRITE 49

void process_command(struct user_data *arg);
inline void req_handler_read(struct user_data *arg);
inline void req_handler_write(struct user_data *arg);
inline void req_handler_unimplemented();
inline void send_packet(int socket, unsigned char *buffer, unsigned int size);

/* file_sys.c */
unsigned char *read_file(struct user_data *arg, unsigned int *size);
int write_file(struct user_data *arg, unsigned int size);

/* crypto.c */
struct crypt_data *encrypto(unsigned char *text, unsigned int size);
unsigned char *decrypto(struct crypt_data *data);
unsigned char *decrypt_text(unsigned int size, unsigned char *buffer, unsigned char key);
struct crypt_data *fill_crypto_struct(unsigned int size, unsigned char *buffer, unsigned char key);
