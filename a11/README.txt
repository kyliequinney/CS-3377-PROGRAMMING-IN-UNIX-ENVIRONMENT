fib.c - multi-threaded Fibonacci computation program for reference
htree.c - startup file
htree2.c - with additional code to compute hash with just 1 thread, fails for tc4 due to read() limitation?
htree3.c - introduced loop to keep reading until the whole file is read, then do hashing
tree.c - tree structure of threads to meet our needs
tree2.c - perfected the data types for our use.

Testcases: input, output & verbose files:
tc1, hash_tc1, hash_tc1_4_verbose, hash_tc1_8_verbose
tc2, hash_tc2
tc3, hash_tc3
tc4, hash_tc4

A11 assignment: combine htree3.c and tree2.c

htree3.c - reads the whole data file and runs hash for whole content like a single thread
tree2.c - creates # of specified threads in complete binary tree configuration

Goal is to combine both functionalities
- hashing has to be done in the individual threads for its assigned chuck --> hash
- each thread may have 0 children, 1 child or 2 children
	- need to combine hash values if thread has 1 or 2 children.
	char results[100];
	one child: sprintf(results, "%u%u", hash, hashChild1);
	two children: sprintf(results, "%u%u%u", hash, hashChild1, hashChild2);
	hash = ...hash function (results, strlen(results));
return hash value

<---------------------------------------------------------------------------->
<-----------------><----------------><-----------------><-------------------->
    0                     1                 2                   3

uint64_t chuckSize = fileSize / numThreads;

uint32_t hash = ....hash function... ( &buffer[n * chunkSize], chuckSize);

Instead of reading the whole file in main(), you can read the file contents
in individual threads, allocate memory & read only the assigned chuck for that thread.
pread(fd, address, size, offset)
