#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define SIBLOCKS 2048L
#define DIBLOCKS (2048L*2048L)
#define TIBLOCKS (2048L*2048L*2048L)

int count = 0;
long computeOverheadBlocks(long diskblocks) {
	//CODE HERE!
	long overheadBlocks = diskblocks;

	//If does not enter extended blocks
	if(overheadBlocks <= 12){
		return 0;
	}
	
	//subtract 12 to fill the 12 initial spots
	overheadBlocks = overheadBlocks - 12;

	//if fit in SI Blocks
	if(overheadBlocks <= SIBLOCKS){
		return 1;
	}

	//if fit in DI Blocks
	else if(overheadBlocks <= DIBLOCKS){
		count = overheadBlocks / 2048;//counts how many SI Blocks it fills within
		count = count + 2;//add two for the initial si and di
		return count;
		}
	
	//if fit in TI Blocks
	else if(overheadBlocks <= (68753047648/8)){//was TIBLOCK, then 8594130956
		double temp;
		temp = (overheadBlocks / 2048.0);
		if(temp <= 2049){
			return 2050;
		}

		temp = temp + (temp / (2048.0 * 2048.0));
		
		double temp2 = temp / 2048.0;
		temp2 = temp2 + temp;
		if((temp2 - (int)temp2) == 1){
			temp2 = temp2 +1;
		}
 
		count = temp2 + 3;//and maybe also +2????
		return count;
	}

	//if too big
	else{
		return -1;
	}
}


int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: diskblocks <file size in KBs>\n");
		return -1;
	}
	long filesize = atol(argv[1]);
	long diskblocks = filesize / 8;
	if (filesize % 8) 
		diskblocks++;

	printf("%ld %ld\n", diskblocks, computeOverheadBlocks(diskblocks));
}
