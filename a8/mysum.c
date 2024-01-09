#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//2 pipes to send data to child, 1 pipe to read the result
//index 0 and 1 for writing reading is index 2
#define MAXPIPES 3

int fds[MAXPIPES][2]; 

void child() {
	//rewire the pipes and invoke the program "add"
	//rewire first pipe read to fd 0
	close(0);
	dup(fds[0][0]); //or dup2(fds[0]0[], 0) combines these two lines
	//rewire second pipe read to fd 3
	close(3);
	dup(fds[1][0]);
	//rewire third pipe write to fd 1
	close(1);
	dup(fds[2][1]);

	execl("./add", "add", NULL); //add "-v" between add and NULL while debugging
	fprintf(stderr, "world is ending :(\n");
}


int main() {
	dup(0); //makes fd 3 busy, so that it won't be used by pipes

	//pipe fd values (3,4), (5,6), (7,8) --> (4,5), (6,7), (8,9)
	for(int i=0; i<MAXPIPES; i++)
		pipe(fds[i]);
	if (!fork())//if child process
		child();
	
	//read the data from the user and pump them to pipes
	int x, y, z;
	printf("Enter 2 numbers:\n");
	while(scanf("%d%d", &x, &y) == 2){
		//write x and y to pipes -> child process
		write(fds[0][1], &x, sizeof(int));
		write(fds[1][1], &y, sizeof(int));
		//read result from pipe <- sent by child
		read(fds[2][0], &z, sizeof(int));
		printf("result: %d\n", z);
	}
	//read the output and print...
}
