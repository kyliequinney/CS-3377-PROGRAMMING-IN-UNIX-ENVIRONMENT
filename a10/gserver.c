#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>

 //read dictionary file to array of words & get ready to play the hangman!
//if you are using fgets() to read the word
//	  cptr = strchr(line, '\n');
//	  if (cptr)
//	  	*cptr = '\0';
// However, since we are dealing with words, you can use fscanf(...,"%s", ...) instead!

#define MAXWORDS 100000
#define MAXLEN 1000

char *words[MAXWORDS];
int numWords = 0;

int main(){
	char line[MAXLEN];

	FILE *fp = fopen("dictionary.txt", "r");
	if(!fp){
		puts("dictionary.txt cannot be opened for reading.");
		exit(1);
	}

	int i = 0;
	//read in dictionary
	while(fgets(line, MAXLEN, fp)){
		//words[i] = line;
		words[i] = (char *) malloc (strlen(line) + 1);
		strcpy(words[i], line);
		i++;
	}
	numWords = i;
	printf("%d words were read.\n", numWords);

	srand(getpid() + time(NULL) + getuid());

	char filename[MAXLEN];
	sprintf(filename, "/tmp/%s-%d", getenv("USER"), getpid());
	mkfifo(filename, 0600);
	chmod(filename, 0622);
	printf("Send your requests to %s\n",filename);

	while(1){
		FILE *fp = fopen(filename, "r");
		if(!fp){
			printf("FIFO %s cannot be opened for reading.\n", filename);
			exit(2);
		}
		printf("Opened %s to read...\n", filename);

 // wait for a request to come in (client specifies unique pipename)
 		while(fgets(line, MAXLEN, fp)){
			char *cptr = strchr(line, '\n');
			if(cptr)
				*cptr = '\0';


			if(!fork()){
				FILE *clientfp = fopen(line, "w");

				char serverfifo[MAXLEN];
				sprintf(serverfifo, "/tmp/%s-%d", getenv("USER"), getpid());
				mkfifo(serverfifo, 0600);
				chmod(serverfifo, 0622);

				fprintf(clientfp, "%s\n", serverfifo);
				fflush(clientfp);

				FILE *serverfp = fopen(serverfifo, "r");

				int index = rand() % numWords;
				//select random word
				char starWord[MAXLEN];
				char ans[MAXLEN];
				strcpy(ans, words[index]);
			
				strcpy(starWord, words[index]);
				int stars = strlen(starWord)-1;
				int wrongGuesses = 0;
				char guess[MAXLEN];
				for(int i = 0; i < strlen(starWord)-1; i++){
					starWord[i] = '*';
				}
				fprintf(clientfp, "Word: %s\n", ans);
				fflush(clientfp);
				while(stars > 0){
					
					fprintf(clientfp, "(Guess) Enter a letter in word %s > ", starWord);
					fflush(clientfp);

					//recieve response (single letter)
					//CHNAGE TO FSCANF NOT FGETS
					fscanf(serverfp, "%s", guess);
					//char *letter = strtok(line, " \n");
					bool match = false;
					for(int i = 0; i < strlen(ans); i++){
						
						if(guess[0] == ans[i]){
							match = true;
							if(guess[0] == starWord[i]){
								fprintf(clientfp, "%s is already in the word.\n", guess);
								fflush(clientfp);
								break;
							}
							else{
								starWord[i] = guess[0];
								stars--;
							}
						}
					}
					if(!match){
						fprintf(clientfp, "%s is not in the word :(\n", guess);
						fflush(clientfp);
						wrongGuesses++;
					}
				}
				fprintf(clientfp, "The word is %s\n", ans);
				fprintf(clientfp, "You missed %d times\n", wrongGuesses); 
				fflush(clientfp);


				fclose(clientfp);
				fclose(serverfp);
				unlink(serverfifo);
				exit(0);
			}
		}
		fclose(fp);
	}
}
