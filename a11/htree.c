#include <stdio.h>     
#include <stdlib.h>   
#include <stdint.h>  
#include <inttypes.h>  
#include <errno.h>     // for EINTR
#include <fcntl.h>     
#include <unistd.h>    
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

// Print out the usage of the program and exit.
void usage(char*);
uint32_t jenkins_one_at_a_time_hash(const uint8_t* , uint64_t );
int numThreads = 0;
uint32_t bSize;
// block size
char *buffer;
#define BSIZE 4096
void *child(void *ptr){
	intptr_t index = (intptr_t) ptr;

	//create left child if needed based on the thread # (bc tree structure
	//same for right if needed
	pthread_t thread1, thread2;
	bool child1 = false, child2 = false;
	if(2*index+1 < numThreads){
		pthread_create(&thread1, NULL, child, (void *) (2*index+1));
		child1 = true;
		if(2*index+1 < numThreads){
			pthread_create(&thread2, NULL, child, (void *) (2*index+2));
			child2 = true;
		}
	}
	char *ptr1 = NULL, *ptr2 = NULL;
	
	//compute hash
	uint32_t hash = jenkins_one_at_a_time_hash(&buffer[index * bSize], bSize);
		
	char temp[100];
	sprintf(temp, "%u\n", hash);
	if(child1){
		pthread_join(thread1,(void **) &ptr1);
		uint32_t hashL = (uint32_t) (intptr_t)ptr1;
	
		if(child2){
			pthread_join(thread2, (void **)&ptr2);
			uint32_t hashR = (uint32_t)(intptr_t) ptr2;//
			sprintf(temp, "%u%u%u", hash, hashL, hashR);
		}
		else{
			sprintf(temp, "%u%u", hash, hashL);
		}
	}
	hash = jenkins_one_at_a_time_hash(temp, strlen(temp));
	return (void*)(intptr_t)hash;
	//uint32_t total = 1 + (uint32_t) ptr1 + (uint32_t) ptr2;
	//printf("Thread %d total %d\n", n, total);
	//return (void *) total;	
}

uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, uint64_t length) 
{
  uint64_t i = 0;
  uint32_t hash = 0;

  while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}


void usage(char* s) 
{
  fprintf(stderr, "Usage: %s filename num_threads \n", s);
  exit(EXIT_FAILURE);
}






int main(int argc, char** argv) 
{
  int32_t fd;

  // input checking 
  if (argc != 3)
    usage(argv[0]);

  // open input file
  fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    perror("open failed");
    exit(EXIT_FAILURE);
  }
  // use fstat to get file size
  struct stat fileInfo;
  if(fstat(fd, &fileInfo)){
	perror("File not found.");
	exit(EXIT_FAILURE);
  }
  uint64_t fileSize = fileInfo.st_size;
  //instead of this, just divide filesize by n to find the size of blocks. then each thread has one block  
  numThreads = atoi(argv[2]);
  
  bSize = fileSize / numThreads;
  //if remainder then add an additional block
  //if(fileSize % BSIZE){
  //	nblocks++;
  //}
  
  buffer = (char*) malloc (fileSize);
  printf("size of blocks = %u \n", bSize);

  char *buffer = (char *) malloc (fileSize);
  if(buffer == NULL){
	perror("Memory allocatation failure.");
	exit(EXIT_FAILURE);
  }
  uint64_t nread = 0;
  ssize_t n = 0;

  //read all data
  while((nread < fileSize) && ((n = read(fd, &buffer[nread], fileSize - nread)) > 0)){
	nread += n;
  }

  if(n < 0) {
	perror("File read error.");
	exit(EXIT_FAILURE);
  }
  clock_t start = clock();

  // calculate hash value of the input file
  //uint32_t hash = jenkins_one_at_a_time_hash(buffer, fileSize);
  pthread_t thread0;

  printf("Number of threads: %d\n", numThreads);
  uint32_t num = 0;
  pthread_create(&thread0, NULL, child, (void *)(intptr_t)num);

  void *ptr;

  pthread_join(thread0, &ptr);
  printf("%u\n", (uint32_t)(intptr_t) ptr);
  //exit(0);


  clock_t end = clock();
  //uint32_t hash = 0;
  //printf("hash value = %u \n", hash);
  printf("time taken = %f \n", (end - start) * 1.0 / CLOCKS_PER_SEC);
  close(fd);
  return EXIT_SUCCESS;
}

