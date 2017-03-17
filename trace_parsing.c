#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char p_id[20];
int SECTOR_PER_PAGE = 8;


int trace_parsing_btrace(FILE *fp, int *start_LPN, int *count) {

	int res = 0;
	char str[1024];
	int start_addr = -1;
	int count_sec = -1;

	fgets(str, sizeof(str), fp);
	while(strstr(str, "C") == NULL || strstr(str, p_id) == NULL){
		if(strstr(str, "CPU") != NULL){
			return 2;	
		}
		fgets(str, sizeof(str), fp);
	}
	if(strstr(str, "W")!=NULL){
		char * ori = strstr (str, "W");
		char * new = strtok (ori, " ");
		int strtok_count =0;

		while(new != NULL) {
			if(strtok_count == 1){
				start_addr = atoi(new);
			}else if(strtok_count == 3){
				count_sec = atoi(new);
			}
			new = strtok(NULL , " ");
			strtok_count ++;
		}
		*start_LPN = start_addr / SECTOR_PER_PAGE;
		if((start_addr + count_sec) % SECTOR_PER_PAGE == 0){
			*count= (start_addr+ count_sec) / SECTOR_PER_PAGE - start_addr /SECTOR_PER_PAGE;
		}else {
			*count = (start_addr + count_sec) / SECTOR_PER_PAGE + 1 - start_addr / SECTOR_PER_PAGE;
		}

		return 1;
	}else if(strstr(str, "R")!=NULL){
		char * ori = strstr(str, "R");
		char * new = strtok(ori, " ");
		int strtok_count = 0;
		while(new != NULL) {
			if(strtok_count == 1){
				start_addr = atoi(new);
			}else if(strtok_count == 3){
				count_sec = atoi(new);
			}
			new = strtok(NULL,  " ");
			strtok_count++;
		}

		*start_LPN = start_addr / SECTOR_PER_PAGE;

		*count = (start_addr + count_sec) / SECTOR_PER_PAGE - start_addr / SECTOR_PER_PAGE;
		if((start_addr + count_sec) % SECTOR_PER_PAGE != 0){
			*count = *count + 1;
		}

		return 0;
	}

	return 2;
}

int main(int argc, char *argv[]){

	FILE *fp;
	char filename[100];
	int result, start_LPN, count;
	int i;

	strcpy(filename, argv[1]);
	strcpy(p_id, argv[2]);
	fp = fopen(filename,"r");

	if(fp == 0){
		printf("prob opening file \n");
		getchar();
		exit(1);
	}

	int read = 0, read_count =0;
	int write =0, write_count =0;;
	while(!feof(fp)){
		result = trace_parsing_btrace(fp, &start_LPN, &count);			
		
		if(result == 1){
			write ++;
			write_count += count;
		}else if(result == 0){
			read ++;
			read_count += count;
		}else if(result == 2){
			printf("%d %d %d %d\nfinish \n", read, write,read_count, write_count);
			fclose(fp);
			exit(1);
		}
		
	}

	printf("%d %d readcount :  %d write count : %d\nfinish \n", read, write,read_count, write_count);
	fclose(fp);

	return 0;
}
