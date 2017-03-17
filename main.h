#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long long int KB;
long long int MB;
long long int GB;

long long int K;
long long int M;
long long int G;

/*
* 	Information with default
*/

/*
* 	Information with default
*/
int NUMBER_PARTITION;
int PARTITION_SIZE;
int SECTOR_SIZE;
int PAGE_SIZE;
int BLOCK_SIZE;

int MTB;

int NUMBER_STREAM;

long long int LOGICAL_FLASH_SIZE;
double OVERPROVISION;

long long int FLASH_SIZE;
long long int BUFFER_SIZE;
int FREE_BLOCK;
int PAGE_PER_BLOCK;
int SECTOR_PER_PAGE;
int PAGE_PER_PARTITION;
int PAGE_PER_PARTITION_32;

// cluster
int CLUSTER_SIZE;
int NUMBER_CLUSTER;
int PARTITION_CLUSTER;	//partition_size/cluster_size

int PARTITION_LIMIT;
int BLOCK_LIMIT;

// aging variables
double AGING_LOGICAL_ADDRESS_SPACE_PERCENTAGE;
double AGING_SEQ_PERCENTAGE;
double AGING_RANDOM_PERCENTAGE;
// aging
double RANDOM_RATE;
double SEQ_RATE;
double RANDOM_MOUNT;
double RANDOM_ARRANGE;
int RANDOM_INCREASE;
int RANDOM_SIZE;
char * AGINGFILE;

int MSR;
int CUTTER;
int AGING_IO;

// trace
double TRACE_LOGICAL_ADDRESS_SPACE_PERCENTAGE;

/*
*	Structures
*/

typedef struct _PB {
	int *PPN2LPN;		// store LPN to PPN
	int *valid;		// page's validity
}_PB;

typedef struct _GC {
	int gc_read;
	int gc_write;
	int gc;				// gc È£Ãâ È½¼ö
}_GC;

// count structure
typedef struct _COUNT {
	int read;
	int write;
	int splitbuf;
	int mergebuf;
	int unread; 
	struct _GC block;
}COUNT;


_PB *PB;
COUNT countNum;
int aging;
FILE *wfp;

// function
int trace_parsing(FILE *fp, int* start_LPN, int *count);
void command_setting(int argc, char *argv[]);
int parsing_size(char * str);

void count_init();
void READ_count(int num);
void WRITE_count(int num);
void MAPREAD_count();
void MAPWRITE_count();
void BLOCKGC_count();
void BLOCKGC_READ_count();
void BLOCKGC_WRITE_count();
void SPLITBUF_count();
void MERGEBUF_count();
