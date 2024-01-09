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

void pexit(char *errmsg) {
	fprintf(stderr, "%s\n", errmsg);
	exit(1);
}

string question[1024]; //send this to all clients
string correctResponse[100] = "100"; //check client response with this string.
int rank = 1;

//function for dedicated server thread
void *child(void *ptr) {
	uint32_t connfd = (uint32_t) ptr;

	//wait for a new question
	//get response from client
	//send result back to client
	//for correct responses, increment rank too.
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
	
	int counter=0;
    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
		counter++;
		printf("connected to client %d.\n", counter);
		
     	pthread_t thread1;
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
