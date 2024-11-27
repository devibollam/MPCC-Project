#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

#define LINE_NUMBER (__LINE__)
#define MAX_CLIENTS 10
#define MAX_USERNAME_LEN 20
#define MAX_PASSWORD_LEN 20
#define REGISTRATION_FILE "../../data/registration.txt"
#define MAX_MSG_LEN 256
#define ENCRYPTION_KEY 0x0F // Encryption key (change this value for better encryption)

int server_socket;

FILE *log_file;

time_t t;

struct Client {
    int socket;
    char username[MAX_USERNAME_LEN];
};

struct Client clients[MAX_CLIENTS];
int num_clients = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void logger_init();
void logger(char *level, char *filename, int line_number, char *message, char *activity, char *event);
void signalHandler(int sig_num);
void encryption(char *text);
void decryption(char *text);
void register_client(char *username, char *password);
int authenticate(char *username, char *password);
void *handle_client(void *arg);
