#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

//read all the quotes from quotes.txt
//when client asks for a motivational quote, select one randomly and send it out.

#define MAXQUOTES 10000
#define MAXLEN 1000

//pointers to store the quotes
char *quotes[MAXQUOTES];
int numQuotes = 0;

//pipes
int fds[MAXLEN][2];
int count = 0;
char *commands[MAXLEN];
//runs a simple command
//cmdname arg1 arg2 arg3 ...
void runCommand(char *command) {
	//split and assemble the arguments and invoke execvp()
	//use strtok(..)
	char *args[MAXLEN];
	count = 0;
	char *token = strtok(command, " \n");
	while(token){
		args[count++] = token;
		token = strtok(NULL, " \n");
	}
//	args[count] = NULL;

	execvp(args[0], args);	
}

//cmd0 | cmd1 | cmd2 | cmd3 | cmd4 ....
void child(int i) {
	dup2(fds[i][0], 0);
	close(fds[i][0]);
	close(fds[i][1]);
	//run ith command
	runCommand(commands[i]);

	perror("error in child\n");
	exit(1);
}

void processLine(char *line) {
	char *pipePtr = strchr(line, '|'); //does this command have | chars?
	char *equalPtr = strchr(line, '='); //does this command have =?
	if (pipePtr) { //not NULL - cmd1 | cmd2 | cmd3 ....
		//command has several sub-commands connected with pipes
		//setup commands array

		count = 0;
		char *token = strtok(line, " |\n");
		while(token){
			commands[count++] = token;
			token = strtok(NULL, " |\n");
		}
		//commands[count] = NULL;
		int pipes[count][2];
		//setup pipes array
		for(int i = 0; i < count; i++){
			pipe(pipes[i]);
		}
		printf("%d\n", count);	
		//printf("count %d\n", count);
		//create children --> invoke child(i) in a loop
		for(int i = 0; i < count; i++){
			if(!fork()){ //in child
				if(i > 0){//not first pipe
					dup2(pipes[i-1][0],0);
			//		dup2(pipes[i-1][1], 1);
			//		close(pipes[i-1][1]);
				}

				if(i != count-1){//not last pipe
				//	dup2(pipes[i][0], 0);
					dup2(pipes[i][1],1);
			//		close(pipes[i][0]);
				}

				for(int i = 0; i < count; i++){
					close(pipes[i][0]);
					close(pipes[i][1]);
				}
				runCommand(commands[i]);
			}
		}


		//cmd0 | cmd1 | cmd2 | cmd3 | cmd4 
	} else if (equalPtr) { 
		//command has = operator, so 2 commands --> 2 processes
		char *command1 = strtok(line, "=");
		char *command2 = strtok(NULL, "\n");

		//create two pipes
		int toparent[2], tochild[2];
		pipe(toparent);
		pipe(tochild);

		if(fork()){
			dup2(toparent[0], 0);
			dup2(tochild[1], 1);
			close(toparent[0]);
			close(toparent[1]);
			close(tochild[0]);
			close(tochild[1]);
			runCommand(command1);
		}
		else{
			dup2(tochild[0], 0);
			dup2(toparent[1], 1);
			close(toparent[0]); close(toparent[1]);
			close(tochild[0]); close(tochild[1]);
			runCommand(command2);
		}
    } else 
		//it is a simple command, no pipe or = character
		runCommand(line);

	exit(0);
}

int main() {
	// load up all the quotes from quotes.txt
	char line[MAXLEN];

	FILE *fp = fopen("quotes.txt", "r");
	if(!fp){
		puts("quotes.txt cannot be opened for reading.");
		exit(1);
	}
	int i = 0;

	//read one line at a time, allocate memory, then copy the line into array
	while(fgets(line, MAXLEN, fp)){
		quotes[i] = (char *) malloc (strlen(line)+1);
		strcpy(quotes[i], line);
		i++;
	}
	numQuotes = i;

	srand(getpid() + time(NULL) + getuid());
	// infinite loop to serve the customer
	//srand(time(NULL));
	
	while (1) {
		//output a random quote to stderr
		fputs(quotes[rand()%numQuotes], stderr);
		fprintf(stderr, "# ");
		//get the user input
		fgets(line, 1000, stdin);

		//spawn a child for taking care of it
		if (fork() == 0) 
			processLine(line);

		//wait the child to finish the job!
		wait(NULL);
	}
}
