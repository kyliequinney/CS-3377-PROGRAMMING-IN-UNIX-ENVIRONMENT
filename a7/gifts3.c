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

struct Player {
	char name[MAXLEN];
	double balance;
} ;

//struct is like class - we can use an array of struct (we can use like an array of objects).
struct Player players[MAXPLAYERS];
int numPlayers = 0; //set when actual player data is loaded

//search the player names array and return the index if specified name is found
//return -1 otherwise.
int findIndex(char *name) {
        for(int i=0; i<numPlayers; i++)
            if(strcmp(name, players[i].name) == 0)
               return i;

        return -1;
}

// use binary data file gifts2.dat to read and store the results.

int main(int argc, char *argv[]) {
	//code here!
	
	//reading data is basically an array of struct
	//numPlayers = fread(players, sizeof(struct Player), MAXPLAYERS, filep);
	//when you read  use "rb" instead of "wb"
	if(! strcmp(argv[1], "new")){
		FILE *filep = fopen("gifts3.dat", "wb");
		int count1 = 0;
		int count2 = 0;
		for(int i = 2; i < argc; i++){
			if(!(i%2)){
				strcpy(players[count1].name, argv[i]);
				count1++;
			}
			else{
				players[count2].balance = atof(argv[i]);
				count2++;
				numPlayers++;
			}
		}
		fwrite(&numPlayers, sizeof(int), 1, filep);
		fwrite(players, sizeof(struct Player), numPlayers, filep);
		fclose(filep);
		for(int i = 0; i < numPlayers; i++){
			printf("%10s: %0.2f\n", players[i].name, players[i].balance);
		}
	}
	else{
		FILE *filep = fopen("gifts3.dat", "rb");
		char buffer[100];
		fread(&numPlayers, sizeof(int), 1, filep);
		fread(players, sizeof(struct Player), numPlayers, filep);
		int giverIndex = findIndex(argv[1]);
		players[giverIndex].balance -= atof(argv[2]);
		int temp;
		for(int i = 2; i < argc; i++){
			temp = findIndex(argv[i]);
			players[temp].balance += (atof(argv[2]) / (argc-3));
		}
		fclose(filep);
		filep = fopen("gifts3.dat", "wb");
		fwrite(&numPlayers, sizeof(int), 1, filep);
		fwrite(players, sizeof(struct Player), numPlayers, filep);
		for(int i = 0; i < numPlayers; i++){
			printf("%10s: %0.2f\n", players[i].name, players[i].balance);
		}
	}


}

