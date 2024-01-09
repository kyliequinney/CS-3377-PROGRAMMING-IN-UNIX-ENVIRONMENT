#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define LINESIZE 16
#define BLOCKSIZE 256 //or 400??

void printDiagonal(int fd, char* currentWord, int index){
	//word = word to be printed during this iteration
	//make the block
	int n = strlen(currentWord);
	char space = ' ';
	char currentBlock[BLOCKSIZE];
	memset(currentBlock, space, BLOCKSIZE);
	int currentPos;

	//if index is odd (1st word, 3rd word, etc) print left to right
	if(index % 2){
		for(int i = 0; i < n; i++){
			currentPos = (LINESIZE + 1) * i;
			currentBlock[currentPos] = currentWord[i];
		}
	}

	//if index is even (2nd word, 4th word, etc.) print from right to left	
	else{
	int letter = n-1;
		for(int i = n; i > 0; i--){
			currentPos = (LINESIZE - 1) * i;
			currentBlock[currentPos] = currentWord[letter];
			letter--;
		}
	}

	write(fd, currentBlock, BLOCKSIZE);
}

//use one command line argument
int main(int argc, char *argv[]) {
	//if no input given
	if (argc < 2) {
		printf("Usage: diagonal <textstring>\n");
		return -1;
	}

	int fd = open("diagonal2.out", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
	//make the blank block

	for(int i = 1; i < argc; i++){
		printDiagonal(fd, argv[i], i);
	}
	close(fd);
	puts("diagonal2.out has been created. Use od -c diagonal2.out to see the contents.");
}
