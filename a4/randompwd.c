#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main (int argc, char *argv[]) {
	
	srand(time(NULL) + getpid());
	int minLen = atoi(argv[1]);
	int maxLen = atoi(argv[2]);
	int minLower = atoi(argv[3]);
	int minUpper = atoi(argv[4]);
	int minDigits = atoi(argv[5]);
	int minSpecial = atoi(argv[6]);
	
	//genearate random chars
	char randLetter;
	
	char randSpecial;

	char specialChars[] = {'!', '@', '#', '$', '%', '^', '&', '*', '+', ':', ';'};
	
	char nums[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

	char all[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '@', '#', '$', '%', '^', '&', '*', '+', ':', ';', 'A', 'B', 'C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','x','y','z'};

	//password output variable
	char password[100];
	int count = 0;

	//create the password with specified number of each char type (letter, num, special)
	//for lowercase char
	for(int i = 0; i < minLower; i++){
		randLetter = 'A' + (rand() % 26);
		randLetter = tolower(randLetter);
		password[i] = randLetter;
		count++;
	}
	
	int tempCount = 0;
	
	//for uppercase char
	for(int i = count; i < (minUpper+count); i++){
		randLetter = 'A' + (rand() % 26);
		password[i] = randLetter;
		tempCount++;
	}	
	
	count += tempCount;
	tempCount = 0;
	
	//for number
	char randNum;
	for(int i = count; i < (minDigits+count); i++){
		randNum = nums[rand() % 10];
		password[i] = randNum;
		tempCount++;
	}
	
	count+=tempCount;
	tempCount = 0;

	//for special char
	for(int i = count; i < (minSpecial+count); i++){
		randSpecial = specialChars[rand() % 10];
		password[i] = randSpecial;
		tempCount++;
	}

	count += tempCount;

	//make sure meets minimum length
	int fillerNums = 0;
	if (count < maxLen){
		int maxFiller = maxLen - count;
		int minFiller = minLen - count;
		fillerNums = (rand() % (maxFiller - minFiller + 1))+minFiller;
		for (int i = 0; i < fillerNums; i++){
			password[count + i] = all[rand() % 72];
		}
	}

	count += fillerNums;

	password[count] = '\0';

	//create a shuffle function that swaps around the indexes of the array
	//code for function can be found through link in email under barcode
	void shuffle (char *array, int n){
		if(n > 1){
			int i;
			int temp = 0;
			while(temp < 5){
				for(i = 0; i < n-1; i++){
					int j = rand() % (n-1);
					char t = array[j];
					array[j] = array[i];
					array[i] = t;
				}
				temp++;
			}
		}
	}

	shuffle(password, count);
	printf("%s\n", password);
	return 1;
	
}
