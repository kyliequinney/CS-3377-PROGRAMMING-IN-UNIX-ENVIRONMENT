#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAXLEN 1000000
char input[MAXLEN];

int main() {
	scanf("%s", input);

	//CODE HERE!
	
	char frontLeg = '(';
	int backLegsCount = 0;
	char backLeg = ')';

	for (int i = 0; i < strlen(input); i++){
		if (input[i] == frontLeg & input[i+1] == frontLeg){
			for (int j = i+2; j < strlen(input); j++){
				if (input[j] == backLeg & input[j+1] == 
				backLeg){
					backLegsCount++;
				}
			}
		}	
	} 
	
	printf("Total possible locations: %d", backLegsCount);
	return 0;
}
