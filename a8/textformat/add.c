#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

//any argument coming in - will be considered as enabling debug output
int main(int argc, char *argv[])
{
	bool debugOutput = false;
	//any parameter enables debug output
	if (argc > 1)
		debugOutput = true;

	FILE *secondInput = fdopen(3, "r");
	int x, y, z;
	//read first input from fd 0
	while (scanf("%d", &x) == 1) {
	//while (read(0, &x, sizeof(int)) > 0) {
		if (debugOutput)
			fprintf(stderr, "%s: Read #1: %d\n", argv[0], x);
		//if (read(3, &y, sizeof(int)) > 0) {
		if (fscanf(secondInput, "%d", &y) == 1) {
			z = x + y;
			//write(1, &z, sizeof(int));
			printf("%d\n", z);
			//fflush(stdout);
			if (debugOutput) {
				fprintf(stderr, "%s: Read #2: %d\n", argv[0], y);
				fprintf(stderr, "%s: Result: %d\n", argv[0], z);
			}
	//replace standard output with pipe output
		} else
			exit(1);
	}
	exit(0);
}	
