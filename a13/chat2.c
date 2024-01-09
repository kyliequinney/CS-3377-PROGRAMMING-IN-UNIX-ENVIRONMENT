#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_CLIENTS 100
#define BUFFER_SZ 2048

unsigned int cli_count = 0;
static int uid = 10;

/* Client structure */
typedef struct {
    struct sockaddr_in addr; /* Client remote address */
    int connfd;              /* Connection file descriptor */
    int uid;                 /* Client unique identifier */
    char name[32];           /* Client name */
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

static char topic[BUFFER_SZ/2];

pthread_mutex_t topic_mutex = PTHREAD_MUTEX_INITIALIZER;

/* The 'strdup' function is not available in the C standard  */
char *_strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p) {
        memcpy(p, s, size);
    }
    return p;
}

/* Add client to queue */
void queue_add(client_t *cl){
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Delete client from queue */
void queue_delete(int uid){
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i]) {
            if (clients[i]->uid == uid) {
                clients[i] = NULL;
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Send message to all clients but the sender */
void send_message(char *s, int uid){
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i]) {
            if (clients[i]->uid != uid) {
                if (write(clients[i]->connfd, s, strlen(s)) < 0) {
                    perror("Write to descriptor failed");
                    break;
                }
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Send message to all clients */
void send_message_all(char *s){
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i <MAX_CLIENTS; ++i){
        if (clients[i]) {
            if (write(clients[i]->connfd, s, strlen(s)) < 0) {
                perror("Write to descriptor failed");
                break;
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Send message to sender */
void send_message_self(const char *s, int connfd){
    if (write(connfd, s, strlen(s)) < 0) {
        perror("Write to descriptor failed");
        exit(-1);
    }
}

/* Send message to client */
void send_message_client(char *s, int uid){
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i){
        if (clients[i]) {
            if (clients[i]->uid == uid) {
                if (write(clients[i]->connfd, s, strlen(s))<0) {
                    perror("Write to descriptor failed");
                    break;
                }
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Send list of active clients */
void send_active_clients(int connfd){
    char s[64];

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i){
        if (clients[i]) {
            sprintf(s, "<< [%d] %s\r\n", clients[i]->uid, clients[i]->name);
            send_message_self(s, connfd);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Strip CRLF */
void strip_newline(char *s){
    while (*s != '\0') {
        if (*s == '\r' || *s == '\n') {
            *s = '\0';
        }
        s++;
    }
}

/* Print ip address */
void print_client_addr(struct sockaddr_in addr){
    printf("%d.%d.%d.%d",
        addr.sin_addr.s_addr & 0xff,
        (addr.sin_addr.s_addr & 0xff00) >> 8,
        (addr.sin_addr.s_addr & 0xff0000) >> 16,
        (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

/* Handle all communication with the client */
void *handle_client(void *arg){
    char buff_out[BUFFER_SZ];
    char buff_in[BUFFER_SZ / 2];
    int rlen;

    cli_count++;
    client_t *cli = (client_t *)arg;

    printf("<< accept ");
    print_client_addr(cli->addr);
    printf(" referenced by %d\n", cli->uid);

    sprintf(buff_out, "<< %s has joined\r\n", cli->name);
    send_message_all(buff_out);

    pthread_mutex_lock(&topic_mutex);
    if (strlen(topic)) {
        buff_out[0] = '\0';
        sprintf(buff_out, "<< topic: %s\r\n", topic);
        send_message_self(buff_out, cli->connfd);
    }
    pthread_mutex_unlock(&topic_mutex);

    send_message_self("<< see /help for assistance\r\n", cli->connfd);

    /* Receive input from client */
    while ((rlen = read(cli->connfd, buff_in, sizeof(buff_in) - 1)) > 0) {
        buff_in[rlen] = '\0';
        buff_out[0] = '\0';
        strip_newline(buff_in);

        /* Ignore empty buffer */
        if (!strlen(buff_in)) {
            continue;
        }

        /* Special options */
        if (buff_in[0] == '/') {
            char *command, *param;
            command = strtok(buff_in," ");
            if (!strcmp(command, "/quit")) {
                break;
            } else if (!strcmp(command, "/ping")) {
                send_message_self("<< pong\r\n", cli->connfd);
            } else if (!strcmp(command, "/topic")) {
                param = strtok(NULL, " ");
                if (param) {
                    pthread_mutex_lock(&topic_mutex);
                    topic[0] = '\0';
                    while (param != NULL) {
                        strcat(topic, param);
                        strcat(topic, " ");
                        param = strtok(NULL, " ");
                    }
                    pthread_mutex_unlock(&topic_mutex);
                    sprintf(buff_out, "<< topic changed to: %s \r\n", topic);
                    send_message_all(buff_out);
                } else {
                    send_message_self("<< message cannot be null\r\n", cli->connfd);
                }
            } else if (!strcmp(command, "/nick")) {
                param = strtok(NULL, " ");
                if (param) {
                    char *old_name = _strdup(cli->name);
                    if (!old_name) {
                        perror("Cannot allocate memory");
                        continue;
                    }
                    strncpy(cli->name, param, sizeof(cli->name));
                    cli->name[sizeof(cli->name)-1] = '\0';
                    sprintf(buff_out, "<< %s is now known as %s\r\n", old_name, cli->name);
                    free(old_name);
                    send_message_all(buff_out);
                } else {
                    send_message_self("<< name cannot be null\r\n", cli->connfd);
                }
            } else if (!strcmp(command, "/msg")) {
                param = strtok(NULL, " ");
                if (param) {
                    int uid = atoi(param);
                    param = strtok(NULL, " ");
                    if (param) {
                        sprintf(buff_out, "[PM][%s]", cli->name);
                        while (param != NULL) {
                            strcat(buff_out, " ");
                            strcat(buff_out, param);
                            param = strtok(NULL, " ");
                        }
                        strcat(buff_out, "\r\n");
                        send_message_client(buff_out, uid);
                    } else {
                        send_message_self("<< message cannot be null\r\n", cli->connfd);
                    }
                } else {
                    send_message_self("<< reference cannot be null\r\n", cli->connfd);
                }
            } else if(!strcmp(command, "/list")) {
                sprintf(buff_out, "<< clients %d\r\n", cli_count);
                send_message_self(buff_out, cli->connfd);
                send_active_clients(cli->connfd);
            } else if (!strcmp(command, "/help")) {
                strcat(buff_out, "<< /quit     Quit chatroom\r\n");
                strcat(buff_out, "<< /ping     Server test\r\n");
                strcat(buff_out, "<< /topic    <message> Set chat topic\r\n");
                strcat(buff_out, "<< /nick     <name> Change nickname\r\n");
                strcat(buff_out, "<< /msg      <reference> <message> Send private message\r\n");
                strcat(buff_out, "<< /list     Show active clients\r\n");
                strcat(buff_out, "<< /help     Show help\r\n");
                send_message_self(buff_out, cli->connfd);
            } else {
                send_message_self("<< unknown command\r\n", cli->connfd);
            }
        } else {
            /* Send message */
            snprintf(buff_out, sizeof(buff_out), "[%s] %s\r\n", cli->name, buff_in);
            send_message(buff_out, cli->uid);
        }
    }

    /* Close connection */
    sprintf(buff_out, "<< %s has left\r\n", cli->name);
    send_message_all(buff_out);
    close(cli->connfd);

    /* Delete client from queue and yield thread */
    queue_delete(cli->uid);
    printf("<< quit ");
    print_client_addr(cli->addr);
    printf(" referenced by %d\n", cli->uid);
    free(cli);
    cli_count--;
    pthread_detach(pthread_self());

    return NULL;
}

int main(int argc, char *argv[]){
   srand(getpid() + time(NULL) + getuid());

        /*while (fgets(questions[availableQuestions], 200, fp)) {
                fgets(answers[availableQuestions], 200, fp);
                char *cptr = strchr(answers[availableQuestions], '\n');
                if (cptr)
                        *cptr = '\0';
                char temp[100];
                fgets(temp, 100, fp);
                availableQuestions++;
        }
        printf("%d questions read.\n", availableQuestions);
        */
        int listenfd = 0, connfd = 0;
        struct sockaddr_in serv_addr;

        char buffer[1025];
        time_t ticks;

        if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        pexit("socket() error.");

        memset(&serv_addr, '0', sizeof(serv_addr));
        memset(buffer, '0', sizeof(buffer));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        int port = 4999;
        do {
                port++;
                serv_addr.sin_port = htons(port);
        } while (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0);
        printf("bind() succeeds for port #%d\n", port);

        if (listen(listenfd, 10) < 0)
                pexit("listen() error.");

        //one more thread to send the questions?
        pthread_t thread1;
        pthread_create(&thread1, NULL, handle_client, NULL);
	int counter=0;
        while(1)
        {
                connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
                counter++;
                fprintf(stderr, "connected to client %d.\n", counter);

                pthread_create(&thread1, NULL, child, (void *) connfd);

                /*
                //read a line from the client and write it back to the client
                while ((n = read(connfd, buffer, sizeof(buffer))) > 0) {
                        //do something based on buffer and send that instead?
                        //simply increment each character! 
                        for(int i=0; i<n; i++)
                                buffer[i]++;
                        write(connfd, buffer, n);
                }

                printf("served client %d.\n", counter);
                //pclose(fcommand);
                close(connfd);
                //sleep(1);
                exit(0); //this is child server process. It is done!
                */
     }
    }

    return EXIT_SUCCESS;
}
