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

#define MAXWORDS 100000
#define MAXLEN 1000

char *words[MAXWORDS];
int numWords=0;

int main() {
	char line[MAXLEN];

	FILE *fp = fopen("dictionary.txt", "r");
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
	
		fclose(fp);
}
