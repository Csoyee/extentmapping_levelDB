#include "mftl.h"
#include "main.h"
#include "random.h"


void READ_count(int num) {
   countNum.read += num;
}

void WRITE_count(int num) {
   countNum.write += num;
}

void BLOCKGC_count() {
   countNum.block.gc++;
}

void BLOCKGC_READ_count(int num) {
   countNum.block.gc_read += num;
}

void BLOCKGC_WRITE_count(int num) {
   countNum.block.gc_write += num;
}

void SPLITBUF_count() {
	countNum.splitbuf ++;
}
void MERGEBUF_count() {
	countNum.mergebuf ++;
}

void count_init() {
   // init count information
   countNum.read = 0;
   countNum.write = 0;
   countNum.block.gc = 0;
   countNum.block.gc_read = 0;
   countNum.block.gc_write = 0;
}

void printCount () {
	fprintf(wfp,"end\n");
	fprintf(wfp, "read: %d, write: %d\nsplit: %d, merge: %d \nblock.gc: %d, block.gc_read: %d, block.gc_write: %d\n"
		,countNum.read, countNum.write
		,countNum.splitbuf, countNum.mergebuf
		, countNum.block.gc, countNum.block.gc_read, countNum.block.gc_write);
}

int parsing_size(char * str) {
	if (strstr(str, "K") != NULL) {
		return KB;
	}
	else if (strstr(str, "M") != NULL) {
		return MB;
	}
	else if (strstr(str, "G") != NULL) {
		return GB;
	}
	else {
		printf("ERROR in parsing_size\n");
		exit(1);
	}
}

void command_setting(int argc, char *argv[]) {
	
	int i, j = 0;

	KB = 1024;
	MB = 1024 * KB;
	GB = 1024 * MB;

	K = 1024;
	M = 1024 * K;
	G = 1024 * M;

	/*
		Default Setting
	*/
	// partition
	//NUMBER_PARTITION = 64 * K;
	NUMBER_PARTITION = 256 * K;

	//PARTITION_SIZE = 1 * MB;
	//PARTITION_SIZE = 4 * MB;
	PARTITION_SIZE = 256 * KB;
	// stream
	NUMBER_STREAM = 8;

	// cluster
	//CLUSTER_SIZE = 2 * MB;
	CLUSTER_SIZE = 1 * MB;


	SECTOR_SIZE = 512;
	PAGE_SIZE = 4 * KB;
	BLOCK_SIZE = 512 * KB;
	BUFFER_SIZE = 7765 * KB;

	//LOGICAL_FLASH_SIZE = 32 * GB;
	LOGICAL_FLASH_SIZE = 64*GB;
	OVERPROVISION = 5952*MB;
	//OVERPROVISION = 3*GB + 6716 * PAGE_SIZE;
	
	AGING_LOGICAL_ADDRESS_SPACE_PERCENTAGE = 0.5;
	AGING_RANDOM_PERCENTAGE = 0.3;
	AGING_SEQ_PERCENTAGE = 0.7;

	TRACE_LOGICAL_ADDRESS_SPACE_PERCENTAGE = 0.5;

	PARTITION_LIMIT = 200;
	BLOCK_LIMIT = 100;

	//def
	RANDOM_RATE = 0.3;
	SEQ_RATE = 0.5;
	RANDOM_MOUNT = 0.6;
	RANDOM_ARRANGE = 0.5;
	RANDOM_INCREASE = 1;
	RANDOM_SIZE = 1;
	MSR = 1;
	CUTTER = 0;
	AGINGFILE = NULL;
	//test_setting();
	MTB = 104;

	for (i = 0; i<argc; i++) {
		
		if(strcmp(argv[i], "-mtb") == 0){
			i++;
			MTB = atoi(argv[i]);
		}

		if (strcmp(argv[i], "-aging") == 0) {
			i++;
			AGINGFILE = argv[i];
		}

		if (strcmp(argv[i], "-cut") == 0) {
			i++;
			CUTTER = atoi(argv[i]);
		}

		if (strcmp(argv[i], "-msr") == 0) {
			i++;
			MSR = atoi(argv[i]);
		}

		if (strcmp(argv[i], "-randr") == 0) {
			i++;
			RANDOM_RATE = atof(argv[i]);
		}

		if (strcmp(argv[i], "-seqr") == 0) {
			i++;
			SEQ_RATE = atof(argv[i]);
		}

		if (strcmp(argv[i], "-randm") == 0) {
			i++;
			RANDOM_MOUNT = atof(argv[i]);
		}

		if (strcmp(argv[i], "-randa") == 0) {
			i++;
			RANDOM_ARRANGE = atof(argv[i]);
		}

		if (strcmp(argv[i], "-randi") == 0) {
			i++;
			RANDOM_INCREASE = atoi(argv[i]);
		}

		if (strcmp(argv[i], "-rands") == 0) {
			i++;
			RANDOM_SIZE = atoi(argv[i]);
		}

		if (strcmp(argv[i], "-stc") == 0) {
			i++;
			NUMBER_STREAM = atoi(argv[i]);
		}

		if (strcmp(argv[i], "-ses") == 0) {
			i++;
			SECTOR_SIZE = atoi(argv[i]);
		}

		if (strcmp(argv[i], "-pas") == 0) {
			i++;
			PARTITION_SIZE = atoi(argv[i])*KB;
		}

		if (strcmp(argv[i], "-pac") == 0) {
			i++;
			NUMBER_PARTITION = atoi(argv[i])*K;
		}

		if (strcmp(argv[i], "-ps") == 0) {
			i++;
			PAGE_SIZE = atoi(argv[i]);
		}

		if (strcmp(argv[i], "-bs") == 0) {
			i++;
			BLOCK_SIZE = atoi(argv[i]);
		}

		if (strcmp(argv[i], "-ss") == 0) {
			i++;
			LOGICAL_FLASH_SIZE = atoi(argv[i])*GB;
		}

		if (strcmp(argv[i], "-op") == 0) {
			i++;
			OVERPROVISION = atoi(argv[i])*MB;
		}
		if  (strcmp(argv[i], "-buf") == 0) {
			i++;
			BUFFER_SIZE = atoi(argv[i]) * KB;
		}
	}
	
	FLASH_SIZE = LOGICAL_FLASH_SIZE + OVERPROVISION;
	FREE_BLOCK = (int)(FLASH_SIZE / (long long int) BLOCK_SIZE);
	
	PAGE_PER_BLOCK = BLOCK_SIZE / PAGE_SIZE;
	SECTOR_PER_PAGE = PAGE_SIZE / SECTOR_SIZE;
	PAGE_PER_PARTITION = PARTITION_SIZE / PAGE_SIZE;
	PAGE_PER_PARTITION_32 = PAGE_PER_PARTITION / 32;
	// DEBUG
	printf("[DEBUG] NUMBER_PARTITION : %lld K\n", NUMBER_PARTITION/K);
	printf("[DEBUG] PARTITION_SIZE : %lld MB\n", PARTITION_SIZE/MB);
	printf("[DEBUG] NUMBER_STREAM : %d\n", NUMBER_STREAM);
	printf("[DEBUG] SECTOR_SIZE : %d Byte\n", SECTOR_SIZE);
	printf("[DEBUG] PAGE_SIZE : %lld KB\n", PAGE_SIZE/KB);
	printf("[DEBUG] BLOCK_SIZE : %lld KB\n", BLOCK_SIZE/KB);
	printf("[DEBUG] LOGICAL_FLASH_SIZE : %lld GB\n", LOGICAL_FLASH_SIZE/GB);
	printf("[DEBUG] OVERPROVISION : %f\n", OVERPROVISION);
	printf("[DEBUG] FLASH_SIZE : %lld GB\n", FLASH_SIZE/GB);
	printf("[DEBUG] FREE_BLOCK : %d\n", FREE_BLOCK);
	printf("[DEBUG] PAGE_PER_BLOCK : %d\n", PAGE_PER_BLOCK);
	printf("[DEBUG] CLUSTER_SIZE : %lld MB\n", CLUSTER_SIZE/MB);
	printf("[DEBUG] NUMBER_CLUSTER : %d\n", NUMBER_CLUSTER);
	printf("[DEBUG] PARTITION_CLUSTER : %d\n", PARTITION_CLUSTER);


	// create the PB map
	PB = (_PB *)malloc(sizeof(_PB) * FREE_BLOCK);

	for (i = 0; i<FREE_BLOCK; i++) {

		PB[i].valid = (int *)malloc(sizeof(int) * PAGE_PER_BLOCK);
		PB[i].PPN2LPN = (int *)malloc(sizeof(int) * PAGE_PER_BLOCK);

		for (j = 0; j<PAGE_PER_BLOCK; j++) {
			PB[i].valid[j] = -1;
			PB[i].PPN2LPN[j] = -1;
		}
	}
}

int trace_parsing_msr(FILE *fp, int *start_LPN, int *count) {
    char str[1024];
    char str2[1024];
    unsigned long long int timestamp;
    char *hostname;
    int disknum;
    char *type;
    unsigned long long offset;
    int size;
    int responsetime;
 
    char *tok = NULL;
 
    fscanf(fp, "%s", str);

    if (str == NULL)
        return 2;
 
    timestamp = atol((tok = strtok(str, ",")));
    if (timestamp == NULL)
        return 2;
    hostname = strtok(NULL, ",");
    if (hostname == NULL)
        return 2;
    disknum = atoi((tok = strtok(NULL, ",")));
    type = strtok(NULL, ",");
    if (type == NULL)
        return 2;
    offset = atoll((tok = strtok(NULL, ",")));
    if (offset == NULL)
        return 2;
    size = atoi((tok = strtok(NULL, ",")));
    if (size == NULL)
        return 2;
    responsetime = atoi((tok = strtok(NULL, ",")));
    if (responsetime == NULL)
        return 2;
 
 
    if (strstr(type, "W")) {
        *start_LPN = offset / PAGE_SIZE;
 
        if ((offset + size) % PAGE_SIZE == 0)
            *count = (offset + size) / PAGE_SIZE - offset / PAGE_SIZE;
        else
            *count = (offset + size) / PAGE_SIZE + 1 - offset / PAGE_SIZE;
 
        return 1;
    }else if (strstr(type, "R")) {
        *start_LPN = offset / PAGE_SIZE;
 
        if ((offset + size) % PAGE_SIZE == 0)
            *count = (offset + size) / PAGE_SIZE - offset / PAGE_SIZE;
        else
            *count = (offset + size) / PAGE_SIZE + 1 - offset / PAGE_SIZE;
 
        return 0;
    }
    else {
        return 2;
    }
	return 3;
}
 
 
int trace_parsing_filebench(FILE *fp, int* start_LPN, int *count) {
	char *opType = NULL;
	int start_addr = -1;
	int count_sec = -1;

	char str[1024];
    	fgets(str, sizeof(str), fp);
	while(strstr(str, "N") != NULL){
		fgets(str, sizeof(str), fp);
	}
	if (strstr(str, "W") != NULL) {

		char * ori = strstr(str, "W");
		char * new = strtok(ori, " ");
		int strtok_count = 0;

		while (new != NULL) {
			if (strtok_count == 1) {
				start_addr = atoi(new);
			}
			else if (strtok_count == 3) {
				count_sec = atoi(new);
			}
			new = strtok(NULL, " ");
			strtok_count++;
		}

		*start_LPN = start_addr / SECTOR_PER_PAGE;

		if ((start_addr + count_sec) % SECTOR_PER_PAGE == 0)
			*count = (start_addr + count_sec) / SECTOR_PER_PAGE - start_addr / SECTOR_PER_PAGE;
		else
			*count = (start_addr + count_sec) / SECTOR_PER_PAGE + 1 - start_addr / SECTOR_PER_PAGE;
		
		return 1;
	}
	else if (strstr(str, "R") != NULL) {
		char * ori = strstr(str, "R");
		char * new = strtok(ori, " ");
		int strtok_count = 0;

		while (new != NULL) {
			if (strtok_count == 1) {
				start_addr = atoi(new);
			}
			else if (strtok_count == 3) {
				count_sec = atoi(new);
			}
			new = strtok(NULL, " ");
			strtok_count++;
		}

		*start_LPN = start_addr / SECTOR_PER_PAGE;

		if ((start_addr + count_sec) % SECTOR_PER_PAGE == 0)
			*count = (start_addr + count_sec) / SECTOR_PER_PAGE - start_addr / SECTOR_PER_PAGE;
		else
			*count = (start_addr + count_sec) / SECTOR_PER_PAGE + 1 - start_addr / SECTOR_PER_PAGE;

		return 0;
	}
	
	printf("\nEND of file\n");
	return 2;

}


int main( int argc,char* argv[])
{
	int i;
	char filename[40];
	FILE *fp, *fp2;
	int start_LPN = -1;
	int count = -1;
	int result;
	int trace_total_write = 0;
	unsigned long long int trace_logical_size ;
	int arr[6] = { 1,2,4,8,16,32};
	char str[256];
    
	strcpy(filename, argv[1]);
	count_init();
	command_setting(argc, argv);

	strcat(traceFileName,filename);
	sprintf(str, "%lld", LOGICAL_FLASH_SIZE);
	strcat(traceFileName,str);
	M_init();
	fprintf(wfp,"%lld\n", BUFFER_SIZE);
	aging = 1;	

	if (AGINGFILE == NULL){
		// seq
		printf("[main] SEQ WRITE\n");
		for (i = 0; i < LOGICAL_FLASH_SIZE / PAGE_SIZE * SEQ_RATE; i++) {
			M_write(i, 1);
			AGING_IO++;
			if (AGING_IO % 10000 == 1)
				printf("-");
		}
		printf("\n[main] RAND WRITE\n");
		RandomInit(1);
		i = 0;
		int j_c = 0;
		//random_increase

		if (RANDOM_INCREASE == 1) {
			int last_page = 0;
			if (RANDOM_SIZE == 0) {
				while (i <= LOGICAL_FLASH_SIZE / PAGE_SIZE * RANDOM_MOUNT) {
					int arr[3] = { 8, 16, 32 };
					int page = IRandom(last_page, LOGICAL_FLASH_SIZE / PAGE_SIZE * RANDOM_RATE);
					int count = IRandom(0, 2);
					M_write(page, arr[count]);
					last_page = page;
					if (last_page >= LOGICAL_FLASH_SIZE / PAGE_SIZE * RANDOM_RATE) {
						last_page = 0;
					}
					i = i + arr[count];
					AGING_IO = AGING_IO + arr[count];
					if (j_c % 10000 == 0) {
						printf("-");
					}
					j_c++;
				}
			}
			else {
				last_page = IRandom(0, LOGICAL_FLASH_SIZE/PAGE_SIZE *RANDOM_RATE);
				while (i <= LOGICAL_FLASH_SIZE / PAGE_SIZE * RANDOM_MOUNT) {
					//int page = IRandom(last_page, LOGICAL_FLASH_SIZE / PAGE_SIZE * RANDOM_RATE);
					int distance = IRandom(0, 7);
					M_write(last_page+distance, RANDOM_SIZE);
					last_page = last_page+distance;
					if (last_page >= LOGICAL_FLASH_SIZE / PAGE_SIZE * RANDOM_RATE) {
						last_page = IRandom(0, LOGICAL_FLASH_SIZE/PAGE_SIZE*RANDOM_RATE);
					}
					i = i + RANDOM_SIZE;
					AGING_IO = AGING_IO + RANDOM_SIZE;
					if (j_c % 10000 == 0) {
						printf("-");
					}
					j_c++;
				}
			}
		}
		else {
			if (RANDOM_SIZE == 0) {
				while (i <= LOGICAL_FLASH_SIZE / PAGE_SIZE * RANDOM_MOUNT) {
					int arr[3] = { 8, 16, 32};
					int page = IRandom(0, LOGICAL_FLASH_SIZE / PAGE_SIZE * RANDOM_RATE);
					int count = IRandom(0, 2);
					M_write(page, arr[count]);
					i = i + arr[count];
					AGING_IO = AGING_IO + arr[count];
					if (j_c % 10000 == 0) {
						printf("-");
					}
					j_c++;

				}
			}
			else {
				while (i <= LOGICAL_FLASH_SIZE / PAGE_SIZE * RANDOM_MOUNT) {
					int page = IRandom(0, LOGICAL_FLASH_SIZE / PAGE_SIZE * RANDOM_RATE);
					M_write(page, RANDOM_SIZE);
					i = i + RANDOM_SIZE;
					AGING_IO = AGING_IO + RANDOM_SIZE;
					if (j_c % 10000 == 0) {
						printf("-");
					}
					j_c++;
				}
			}
		}
	}else {
		fp2 = fopen(AGINGFILE, "r");
		while (!feof(fp2)) {
			//trace_total_write++;
			result = trace_parsing_filebench(fp2, &start_LPN, &count);
			//}

			if (result == 1) {
				if (start_LPN + count < (LOGICAL_FLASH_SIZE / PAGE_SIZE)) {
					trace_total_write = trace_total_write + count;
					AGING_IO = AGING_IO + count;
					M_write(start_LPN, count);
				}
			}
			else if (result == 0) {

				if (start_LPN + count < (LOGICAL_FLASH_SIZE / PAGE_SIZE)) {
					//read(start_LPN, count);
				}
			}
			else if (result == 3) {

			}
			else {
				break;
			}

			if (trace_total_write % 5000 == 1)
				printf("-");

			if (CUTTER == 1) {
				if (trace_total_write > (10 * G) / PAGE_SIZE) {
					break;
				}
			}
		}
	}
	printf("\n[main] AGING FINN\n");


	fprintf(wfp, "start\n");	
	i = 0;
	
	if((fp =fopen(filename, "r")) == 0){
		printf("prob opening file %d\n",i);
		getchar();
		exit(1);
	}
	i = 0;
	trace_total_write = 0;

	count_init();

	while (!feof(fp)) {
		if (MSR == 1) {
			result = trace_parsing_msr(fp, &start_LPN, &count);
		}
		else {
			result = trace_parsing_filebench(fp, &start_LPN, &count);
		}
		if (result == 1) {
			if (start_LPN + count < (LOGICAL_FLASH_SIZE/PAGE_SIZE)-MTB*PAGE_PER_BLOCK) {
				trace_total_write += count;
				WRITE_count(count);
				M_write(start_LPN, count);
			}
		}
		else if (result == 0) {
			if (start_LPN + count < (LOGICAL_FLASH_SIZE/PAGE_SIZE)-MTB*PAGE_PER_BLOCK) {
				READ_count(count);
				M_read(start_LPN, count);
			}
		}
		else if(result == 2){
			printCount ();
			printf("finish \n");
			fclose(fp);
			fclose(wfp);
			exit(1);
		}else if(result == 3){
			printCount ();
			printf("unexpected !!!\n");
			getchar();
			exit(1);
		}

		//if (trace_total_write % (256 * K) == 0) {
		//	i++;
		//}

		//if(CUTTER == 1){
			//if (trace_total_write > (40 * G) / PAGE_SIZE) {
			//	break;
			//}
		//}
	}

	//printCount ();
	printCount ();
	fclose(fp);
	printf("FINISH!!\n");
	getchar();
	return 0;
}
