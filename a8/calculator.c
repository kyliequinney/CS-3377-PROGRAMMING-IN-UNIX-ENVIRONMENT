#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLEN 1000

char operators[100];
int fds[201][2];
int operatorCount=0;
int numPipes=0;

//char *ops[] = {NULL, "./add", "./subtract", "./mult", "./};
void child(char charector, int index) {
	//CODE HERE!
	////rewire first pipe read to fd 0
	dup2(fds[index*2][0], 0);
	//rewire second pipe read to fd 3
	dup2(fds[index*2+1][0], 3);
	//rewire 3rd pipe write to fd 1
	dup2(fds[(index*2)+2][1], 1);
	
	for(int i = 0; i < numPipes; i++){
		close(fds[i][0]);
		close(fds[i][1]);
	}

	if(charector == '+'){
		execl("./add", "./add", NULL);
		perror("execl");
	}
	if (charector == '-'){
		execl("./subtract", "./subtract", NULL);
		perror("execl");
	}
	if (charector == '*'){
		execl("./multiply", "./multiply", NULL);
		perror("execl");
	}
	if (charector == '/'){
		execl("./divide", "./divide", NULL);
		perror("execl");
	}
	fprintf(stderr, "world is ending :(\n");
	exit(1);
	//reconfigure appropriate pipes (plumbing) to fds 0, 3 and 1
	//close all pipes! IF missed, you will end up with lot of orphans! 
	// operators[index] +         -        *            /
	// execl            add       subtract mult         divide
	//printf() is bad idea! --> fprintf(stderr, "...\n", 
}

int main(int argc, char *argv[]) {
	char line[MAXLEN], *temp;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	FILE *dataFile = fopen(argv[1], "r");
	//read the first line - it contains the configuration
	fgets(line, MAXLEN, dataFile); 
	// sample content for line: a + b - c
	temp = strtok(line, " \n"); //skip the symbol representing variable/parameter
	while (temp = strtok(NULL, " \n")) {
		operators[operatorCount] = temp[0];
		operatorCount++;
		strtok(NULL, " \n"); //skip the symbol representing variable/parameter
	}
	//create the necessary # of pipes
	numPipes = operatorCount * 2 + 1;

	//CODE HERE!
	//loop: create that many pipes (numPipes) -- all pipes are created!
	for(int i = 0; i < numPipes; i++){
		pipe(fds[i]);
	}
	//loop: create that many children (operatorCount)
	for(int i = 0; i < operatorCount; i++){
		if(!fork()){
			child(operators[i],i);

		}
	}
	for(int i = 0; i < operatorCount; i++){
		close(fds[i*2][0]);
		close(fds[i*2+1][0]);
	}


	
	//proceed to read data from the file and keep pumping them into pipes and read result 
	//while (fscanf(dataFile, "%d", &value) > 0)
	int value;
	int result;
	while(fscanf(dataFile, "%d", &value) > 0){
	//	write to first pipe
		write(fds[0][1], &value, sizeof(int));
	//	use loop to read remaining data in that line & keep puming to pipes
		for(int i = 0; i < operatorCount; i ++){
			fscanf(dataFile, "%d", &value);
			write(fds[i*2+1][1], &value, sizeof(int));
		}

	//	read the final result from the final pipe & print
		read(fds[operatorCount*2][0], &value, sizeof(int));
		printf("%d\n", value);
	}


	fclose(dataFile);
	return 0;
}

