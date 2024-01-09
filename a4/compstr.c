#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int main (int argc, char *argv[]) {
	
	//initialize string vars
	char str1[100];
	char str2[100];

	//prompt for 2 strings
	printf("Enter String #1: ");
	fgets(str1, 100, stdin);
	
	printf("Enter String #2: ");
	fgets(str2, 100, stdin);

	//convert to lowercase and remove whitspace
	char newStr1[100];
	char newStr2[100];
	int count = 0;
	for(int i = 0; str1[i]; i++){
		if(str1[i] != ' '){
			newStr1[count] = tolower(str1[i]);
			count++;
		}
	}
	count = 0;
	for(int i = 0; str2[i]; i++){
		str2[i] = tolower(str2[i]);
		if(str2[i] != ' '){
			newStr2[count] = tolower(str2[i]);
			count++;
		}
	}
	
	//compare strings
	if(strcmp(newStr1, newStr2) == 0){
		printf("Both strings are same!");
	}
	else{
		printf("Both strings are different!");
	}
	
	return 1;
	
}
