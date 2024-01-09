#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

//let us assume that there will not be more than 100 players
#define MAXPLAYERS 100
//let us assume that max length for any name is 100 characters
#define MAXLEN 100


//arrays to store the player names and balances
char names[MAXPLAYERS][MAXLEN]; //so each row is a player?
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

int main(int argc, char *argv[]) {
	//code here! use the following code just as reference 

	FILE *filep;
	//if argv[1] == "new" start a new file
	if (! strcmp(argv[1], "new")){
		FILE *filep = fopen("gifts1.txt", "w");
		int count1 = 0;
		int count2 = 0;
		//add each name and score to the arrays
		for(int i = 2; i < argc; i++){
			if (!(i%2)) {
				//add name to names array
				strcpy(names[count1], argv[i]);
				count1++;
			}
			else{
				//enter each score in balances array
				balances[count2] = atof(argv[i]);
				count2++;
			}
		}
		for(int i = 0; i < count2; i++){
			fprintf(filep, "%s\n%.2f\n", names[i], balances[i]);
			printf("%10s: %6.2f\n", names[i], balances[i]);
		}
		fclose(filep);
	}
	//else read in name and score until end of input
	else{
		filep = fopen("gifts1.txt", "r");
		//input = 0: command, 1: name of giver, 2: amount to disperse, rest: recipients
		char buffer[100];
		int count = 0;
		while(fgets(buffer, sizeof(buffer), filep) != NULL){
			if(count % 2 == 0){
				sscanf(buffer, "%s", names[count/2]);
			}
			
			else{
				sscanf(buffer, "%lf", &balances[count/2]);
			
			}
			count++;
		}
		fclose(filep);

		filep = fopen("gifts1.txt", "w");
		numPlayers = count/2;
		int giverIndex = findIndex(argv[1]);
		balances[giverIndex] -= atof(argv[2]);
		int temp;
		for(int i = 2; i < argc; i++){
			temp = findIndex(argv[i]);
			balances[temp] += (atof(argv[2]) / (argc-3));
		}
		for(int i = 0; i < numPlayers; i++){
			fprintf(filep, "%s\n%.2f\n", names[i], balances[i]);
			printf("%10s: %6.2f\n", names[i], balances[i]);
		}
		fclose(filep);
	}
}
