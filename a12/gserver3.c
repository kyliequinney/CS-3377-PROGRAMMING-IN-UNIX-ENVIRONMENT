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

char *question = "", *answer = "";
int rank = 1;

int clientfds[1024]; //stores the connection fds for each client
char clientNames[1024][200]; //stores the connection fds for each client
int correct[1024];
int nextClientIndex = 0;

//responsible for getting question & answer from the user
//sending the questions to all clients
char questions[100][200], answers[100][200];
int availableQuestions = 0;

void *manager_child(void *ptr) {
	
	char ok[100];
	srand(time(NULL));

	for(int q=1; q<10; q++) {
		printf("Press Enter to send question %d\n", q);
		fgets(ok, 100, stdin);
		int qNum = rand() % availableQuestions;
		question = questions[qNum];
		answer = answers[qNum];

		//send the question to all clients!
		for(int i=0; i<nextClientIndex; i++)
			write(clientfds[i], question, strlen(question));
		rank = 1;
	}
	puts("Press Enter to finish.");
		fgets(ok, 100, stdin);
	for(int i=0; i<nextClientIndex; i++)
		printf("%s %d\n", clientNames[i], correct[i]);
}

//function for dedicated server thread
void *child(void *ptr) {
	uint32_t connfd = (uint32_t) ptr;
	int index = nextClientIndex++;
	clientfds[index] = connfd;
	char *prompt = "Enter your name: \n";
	write(connfd, prompt, strlen(prompt));
	read(connfd, clientNames[index], 200);
	char *cptr = strchr(clientNames[index], '\n');
	if (cptr)
		*cptr = '\0';
	char buffer[1024];
	//wait for answer from client
	while (read(connfd, buffer, 1024)) {
		buffer[1023] = '\0';
		char *token = strtok(buffer, " \n");
		if (strcasecmp(token, answer)) //mismatch?
			sprintf(buffer, "Incorrect answer.\n");
		else {
			sprintf(buffer, "Correct answer. Your rank is %d\n", rank++);
			correct[index]++;
		}
		//send result back to client
		//for correct answers, increment rank too.
		write(connfd, buffer, strlen(buffer));
	}
}

int main(int argc, char *argv[])
{
	FILE *fp = fopen("quiz.txt", "r");
	while (fgets(questions[availableQuestions], 200, fp)) {
		fgets(answers[availableQuestions], 200, fp);
		char *cptr = strchr(answers[availableQuestions], '\n');
		if (cptr)
			*cptr = '\0';
		char temp[100];
		fgets(temp, 100, fp);
		availableQuestions++;
	}
	printf("%d questions read.\n", availableQuestions);

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
