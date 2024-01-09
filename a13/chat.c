#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <pthread.h>
#include <string.h>
#include <strings.h>

void pexit(char *errmsg) {
	fprintf(stderr, "%s\n", errmsg);
	exit(1);
}

int clientfds[1024]; //stores the connection fds for each client
char clientNames[1024][200]; //stores the connection fds for each client
//int correct[1024];
int nextClientIndex = 0;


void *manager_child(void *ptr) {
	
	char ok[100];
	srand(time(NULL));

	//for(int q=1; q<10; q++) {
	printf("Chat room has been created!\n");
	fgets(ok, 100, stdin);
	
}
void sendToSelf(const char *s, uint32_t connfd){
	if(write(connfd, s, strlen(s)) < 0){
		perror("Write failed");
		exit(-1);
	}
}

void printList(uint32_t connfd){
	char s[64];
	for(int i = 0; i < nextClientIndex; i++){
		if(clientfds[i]){
			sprintf(s, "<<[%d] %s\r\n", clientfds[i], clientNames[i]); 
			sendToSelf(s, connfd);
		}
	}
}
//function for dedicated server thread
void *child(void *ptr) {
	uint32_t connfd = (intptr_t) ptr;
	int index = nextClientIndex++;
	clientfds[index] = connfd;
	char *welcome = "Welcome!\n";
	write(connfd, welcome, strlen(welcome));
	char *prompt = "Enter your name: \n";
	write(connfd, prompt, strlen(prompt));
	read(connfd, clientNames[index], 200);
	
	char *menu = "Usage:\nsend <user> <message>	#send your message to the specified user\nlist			#output list of all current users\ncheck <user>		#check if specified user is active\n";

	write(connfd, menu, strlen(menu));
	char *space = " ";
	char *newLine = "\n";
	char *cptr = strchr(clientNames[index], '\n');
	if (cptr)
		*cptr = '\0';
	char buffer[1024];
	//wait for answer from client
	while (read(connfd, buffer, 1024)) {
		buffer[1023] = '\0';
		char *token = strtok(buffer, " \n");
//		write(connfd, token[0], strlen(token[0]));
		//write(connfd, token, strlen(token));
		if (!strcmp(token, "send")){ //mismatch?	
			//send message
			char *user = strtok(NULL, " \n");
			int online = 0;	
			int recieve = 0;
			for(int i=0; i < nextClientIndex; i++){
				char *tempUser = clientNames[i];
				if(!strcmp(user, tempUser)){
					recieve = clientfds[i];
					online = 1;
					char *message = strtok(NULL, " \n");
					while(message != NULL){	
						write(recieve, message, strlen(message));
						write(recieve, space, strlen(space));
						message = strtok(NULL, " \n");
					}
					write(recieve, newLine, strlen(newLine));
				}
			}
			if(!online){
				char *nOMess = "user is not online\n";
				write(connfd, nOMess, strlen(nOMess));
			}
			
		}
		else if (!strcmp(token, "list")) {
			//list users
			//write(connfd, index, strlen(index));
			for(int i = 0; i < nextClientIndex; i++){
				char *tempUser = clientNames[i];
				write(connfd, tempUser, strlen(tempUser));
				write(connfd, newLine, strlen(newLine));
			}
		}
		else if(!strcmp(token, "check")){
			//check for user
			int online = 0;
			token = strtok(NULL, " \n");
			for(int i=0; i < nextClientIndex; i++){
				char *tempUser = clientNames[i];
				if(!strcmp(token, tempUser)){
					char *oMess = " is online\n";
					write(connfd, tempUser, strlen(tempUser));
					write(connfd, oMess, strlen(oMess));
					online = 1;
				}
			}
			if(!online){
				char *nOMess = "user is not online\n";
				write(connfd, nOMess, strlen(nOMess));
			}
		}

	}
}

int main(int argc, char *argv[])
{

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
    pthread_create(&thread1, NULL, manager_child, NULL);
	
	int counter=0;
    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
		counter++;
		fprintf(stderr, "connected to client %d.\n", counter);
		
        pthread_create(&thread1, NULL, child, (void *) connfd);

     }
}
