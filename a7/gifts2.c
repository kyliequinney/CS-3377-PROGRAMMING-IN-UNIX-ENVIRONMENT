#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//let us assume that there will not be more than 100 players
#define MAXPLAYERS 100
//let us assume that max length for any name is 100 characters
#define MAXLEN 100

//arrays to store the player names and balances
char names[MAXPLAYERS][MAXLEN];
double balances[MAXPLAYERS];
int numPlayers = 0; //set when actual player data is loaded

//search the player names array and return the index if specified name is found
//return -1 otherwise.
int findIndex(char *name) {
        for(int i=0; i<numPlayers; i++)
            if(strcmp(name, names[i]) == 0)
               return i;

        return -1;
}

// use binary data file gifts2.dat to read and store the results.

int main(int argc, char *argv[]) {
	//code here!
	//following line makes sense only for writing to file
	//if command is not "new" then you would do open and O_RDONLY as ur only command thingy
	if(! strcmp(argv[1], "new")){
		int fd = open("gifts2.dat", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
		int count1 = 0;
		int count2 = 0;
		for(int i = 2; i < argc; i++){
			if(!(i%2)){
				strcpy(names[count1], argv[i]);
				count1++;
				numPlayers++;
			}
			else{
				balances[count2] = atof(argv[i]);
				count2++;
			}
		}

		write(fd, &numPlayers, sizeof(int));
		for(int i = 0 ; i < count2; i++){
			int nameLength = strlen(names[i]);
			write(fd, &nameLength, sizeof(int));
			write(fd, names[i], nameLength);
			write(fd, &balances[i], sizeof(double));
			printf("%10s: %6.2f\n", names[i], balances[i]);
		}
		close(fd);
	}
	else{
		int fd = open("gifts2.dat", O_RDONLY);
		read(fd, &numPlayers, sizeof(int));
		int nameLength;
		for(int i = 0; i < numPlayers; i++){
			read(fd, &nameLength, sizeof(int));
			read(fd, &names[i], nameLength);
			read(fd, &balances[i], sizeof(double));
		}
		close(fd);
		int giverIndex = findIndex(argv[1]);
		balances[giverIndex] -= atof(argv[2]);
		int temp;
		for(int i = 2; i < argc; i++){
			temp = findIndex(argv[i]);
			balances[temp] += (atof(argv[2]) / (argc-3));
		}
		fd = open("gifts2.dat", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
		write(fd, &numPlayers, sizeof(int));
		for(int i = 0 ; i < numPlayers; i++){
			int nameLength = strlen(names[i]);
			write(fd, &nameLength, sizeof(int));
			write(fd, names[i], nameLength);
			write(fd, &balances[i], sizeof(double));
			printf("%10s: %6.2f\n", names[i], balances[i]);
		}
		close(fd);
	}
	//how to write and read individual name
	//simplest approach is to write the name length first, then the actual content
	//to make reading the name from file easier
}
