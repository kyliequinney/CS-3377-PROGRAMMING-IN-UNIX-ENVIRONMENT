/* make up unique pipename for client
 * send it to the server (public pipe)
 * get another unique pipename from the server
 * repeat
 * 	read a line including starred word from the client pipe
 *  display that line to the user 
 *  check whether game is over?
 *  get the user's guess letter
 *  send to the server using server pipe
 */

// custom client for this specific server
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAXLEN 1000

int main(int argc, char *argv[]){

	if(argc != 2){
		puts("Usage: gclient <server-fifo-name>");
		exit(1);
	}

	//server fifo is at argv[1]
	
	char clientfifo[MAXLEN];
	sprintf(clientfifo, "/tmp/%s-%d", getenv("USER"), getpid());
	mkfifo(clientfifo, 0600);
	chmod(clientfifo, 0622);
	
	FILE *fp = fopen(argv[1], "w");

	fprintf(fp, "%s\n", clientfifo);
	fclose(fp);

	FILE *clientfp = fopen(clientfifo, "r");
	char serverfifo[MAXLEN];

	fscanf(clientfp, "%s", serverfifo);
	char line[MAXLEN];
	fgets(line, MAXLEN, clientfp);

	FILE *serverfp = fopen(serverfifo, "w");


	if(fork())
		while (1){
			//read a line from server // if read fails, break;
			// fgets() returns NULL when it fails
			//if that line contains "Enter a" // strstr(line, "Enter a")
			fgets(line, MAXLEN, stdin);
			//char *cptr = strchr(line, '\n');
			//if (cptr)
			//	*cptr = '\0';
			//send one char to server
			fprintf(serverfp, "%s\n", line);
			fflush(serverfp);
		}
	else{
		while(1){
			if(!fgets(line, MAXLEN, fp))
				break;
			printf(line);
		}
	}
	//fclose(serverfp);
	fclose(clientfp);
	unlink(clientfifo);
		//	printf(line);
		//	if line contains "The word is"
		//		break;

//	unlink(...);
}
