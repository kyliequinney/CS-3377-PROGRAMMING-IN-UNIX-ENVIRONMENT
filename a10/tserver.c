#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>

//read all the words from words.txt
//when client asks for a motivational quote, select one randomly and send it out.

#define MAXWORDS 10000
#define MAXLEN 1000

char *words[MAXWORDS];
int numWords=0;

int main() {
	char line[MAXLEN];

	FILE *fp = fopen("wordle.txt", "r");
	if (!fp) {
		puts("wordle.txt cannot be opened for reading.");
		exit(1);
	}
	int i=0;

	//read one line at a time, allocate memory, then copy the line into array
	while (fgets(line, MAXLEN, fp)) {
		words[i] = (char *) malloc (strlen(line)+1);
		strcpy(words[i], line);
		i++;
	}
	numWords = i;
	printf("%d words were read.\n", numWords);

	srand(getpid() + time(NULL) + getuid());
	// puts(words[rand() % numWords]);
	
	//create a named pipes to read client's requests
	char filename[MAXLEN];
	sprintf(filename, "/tmp/%s-%d", getenv("USER"), getpid());
	mkfifo(filename, 0600);
	chmod(filename, 0622);
	printf("Send your requests to %s\n", filename);

	while (1) {
		FILE *fp = fopen(filename, "r");
		if (!fp) {
			printf("FIFO %s cannot be opened for reading.\n", filename);
			exit(2);
		}
		printf("Opened %s to read...\n", filename);

		//wait for clients' requests
		while (fgets(line, MAXLEN, fp)) {

			char *cptr = strchr(line, '\n');
			if (cptr) 
				*cptr = '\0';
	
			int index  = rand() % numWords;
			//create a child to work with this client
			if (fork() == 0) {
				FILE *clientfp = fopen(line, "w");
				//create and send new server fifo to the client
				//for private one-on-one communcations
				char serverfifo[MAXLEN];
				sprintf(serverfifo, "/tmp/%s-%d", getenv("USER"), getpid());
				mkfifo(serverfifo, 0600);
				chmod(serverfifo, 0622);

				fprintf(clientfp, "%s\n", serverfifo);
				fflush(clientfp);

				FILE *serverfp = fopen(serverfifo, "r");

				//select a random word from list, then come with its starred word
				//int index  = rand() % numWords;
				char starredWord[MAXLEN];
				strcpy(starredWord, words[index]);
				for(int i=0; i<3; i++) //put * in 3 random places
					starredWord[rand()%5] = '*';
				fprintf(clientfp, "%s\n", starredWord);
				fflush(clientfp);
				
				//receive the response. It can be like: trick treat foods 
				fgets(line, MAXLEN, serverfp);
				int wordCount = 0;
				bool matched = false; //requires #include <stdbool.h>
				char *word = strtok(line, " \n"); //tokenize and check each word
				while (word) {
					wordCount++;
					if (!strcmp(word, words[index]))
						matched = true;
					word = strtok(NULL, " \n");
				}

				int score = 0;
				if (matched)
					score = 100 / wordCount;
				fprintf(clientfp, "Word: %s Your score: %d\n", words[index], score);
				fclose(clientfp);
				fclose(serverfp);
				
				unlink(serverfifo);
				exit(0);
				/*
				 * int total=0; //for running total for this client
				while (fgets(line, MAXLEN, serverfp)) {
					//example for line: "5 17 7 14\n"
					char *token = strtok(line, " \n"); //get first token
					while (token) { //is token valid?
						//process value atoi(token)
						int number = atoi(token);
						total += number;
						fprintf(clientfp, "Total: %d\n", total); //to client
						printf("Total: %d\n", total); //local print by server

						token = strtok(NULL, " \n"); //get next token
					}
					
					fflush(clientfp);
				}
				*/

			}
		}
		fclose(fp);
	}
}
