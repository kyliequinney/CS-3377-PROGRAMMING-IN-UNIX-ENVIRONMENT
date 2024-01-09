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
	if(overheadBlocks <= 12){
		return 0;//WORKS
	}

	else if(overheadBlocks >= 8750000000){
		return -1;//WORKS
	}

	else if(overheadBlocks > 12){
		overheadBlocks = overheadBlocks - 12;
		if(overheadBlocks <= 2048){
			return 1;//WORKS
		}
		else {
			count = overheadBlocks / 2048;
			count = count + 2;
			
			if((count) <= 2050){
				//printf("%d\n", count);
				return (count);//should return 2 + count
			}
			if(count > 2050){
				count = count + 2;
				//printf("%d\n", count);
				return(count);
			}
		}
	}
	
	return -2;//should retunr 0... or maybe -1??
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
