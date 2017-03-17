#include <stdio.h>
#include <stdlib.h>
#include "mftl.h"
#include "main.h"

_Extent * ExtentMap;


int find_victim() {
	int i, max, res;

	max =PBinv[0];
	res =0;

	for(i=1; i<BLOCK_PER_FLASH ;i++){
		if(max < PBinv[i]){
			max = PBinv[i];
			res = i;	
		}
	}
	return res;
}

void M_init(){

	int i, j;
	int PAGE_NUM;
	
	BLOCK_PER_FLASH = FLASH_SIZE / BLOCK_SIZE;

	PBinv = (int *)malloc(sizeof(int) * BLOCK_PER_FLASH);

	for(i=0 ; i<BLOCK_PER_FLASH; i++){
		PB[i].valid = (int * )malloc(sizeof(int) * PAGE_PER_BLOCK);
		PB[i].PPN2LPN = (int *) malloc(sizeof(int) * PAGE_PER_BLOCK);
		PBinv[i] = -1 ; // -1 free block.
		for(j=0 ; j<PAGE_PER_BLOCK ; j++){
			PB[i].PPN2LPN[j] = -1;
			PB[i].valid[j] = -1;
		}
	}
	// set update block
	udata.block_num=0;
	udata.offset =0;
	PBinv[0] = 0;

	PAGE_NUM = PAGE_PER_BLOCK * BLOCK_PER_FLASH;
	ExtentMap = (_Extent *)malloc(sizeof(_Extent)*PAGE_NUM);

	for(i=0 ; i<PAGE_NUM ; i++){
		ExtentMap[i].lpn = -1;
	}
	wfp = fopen(traceFileName, "w");
}

void M_read(int lpn, int num){

	/*
	 * 1. check map4map (check for start lpn-->key) Lpn2Ppn[lpn] --> check start lpn 
	 *    ( check how many extent is involved in read request? )
	 * 2. check cache --> find_tree(start lpn from 1, 'p')
	 * 3. (if not 2) check map and read at nand --> Lpn2Ppn[lpn] --> pbn & offset
	 */
	int tempLpn, last, rc;
	_Extent tempExtent;
	
	tempLpn = lpn;
	last = lpn + num;
	while( lpn <= tempLpn && tempLpn < last ){
		// check if it is in cache
		int key = tempLpn;
		tempExtent = ExtentMap[tempLpn];
		if(tempExtent.lpn == -1){
			tempLpn ++;
			while(ExtentMap[tempLpn].lpn == -1){
				tempLpn++;		
			}
			continue;
		}
	// search!!
		fprintf(wfp, "read\n%d %d\n", tempExtent.lpn, tempExtent.ppn);
		tempLpn = tempLpn + tempExtent.num;
		num -= tempExtent.num;
	}
}

int search_free_block(){

	int num, i;
	for(i=0 ; i<BLOCK_PER_FLASH  ;i++){
		if(PBinv[i] == -1){
			PBinv[i]++;
			FREE_BLOCK --;
			return i;
		}
	}

	M_GC();
	
	for(i=0 ; i<BLOCK_PER_FLASH  ;i++){
		if(PBinv[i] == -1){
			PBinv[i]++;
			FREE_BLOCK --;
			return i;
		}
	}
	printf("break\n");
	return -1;
}
void block_debug(){

	int i,j;
	int bit_valid;
	for(i=0;i<BLOCK_PER_FLASH;i++){
		bit_valid = 0;
		for(j=0;j<128;j++){
			if(PB[i].valid[j] == 1){
				bit_valid++;
			}
		}
		
		int invalid = 0;
		if(PBinv[i] == -1){
			invalid = 128;
		}else{
			invalid = PBinv[i];
		}
		if(128 - bit_valid != invalid && i!= udata.block_num){
			printf("%d  error : block %d, bit valid : %d PBinv : %d,%d\n", countNum.write, i, 128 - bit_valid, PBinv[i], invalid);
			exit(1);
		}

		//printf("%d - %d\n", i, bit_valid);

		//if(i == 3)
		//	exit(1);
	
	}
}

void allocate_page(int lpn, int num){

	int i, count=0, bn, templpn, offset, startppn, flag =0, mergenum, mergelpn;
	int nlevelmerge=0;
	// allocate data && map(according to data)
	// udata --> last page of update block than search_free_block(1)
	templpn = lpn;
	do{
		num -= count;
		count =0;
		bn = udata.block_num;
		offset = udata.offset;
		for(i=0 ; i<num; i++){
			if(i+offset == PAGE_PER_BLOCK){
				udata.offset = 0;
				udata.block_num = search_free_block();
				flag= 1;
				break;
			}
			
			if(i==0){
				startppn = PAGE_PER_BLOCK * bn + offset;
			}
			PB[bn].valid[offset + i] = 1;
			PB[bn].PPN2LPN[offset + i] = templpn+count;
			count ++;
		}
		if(flag ==0){
			udata.offset = offset+i;
		}else{
			flag= 0;
		}
		if(count != 0){
			if(templpn >0 && ExtentMap[templpn-1].lpn != -1 && ExtentMap[templpn-1].ppn/PAGE_PER_BLOCK == startppn/PAGE_PER_BLOCK && startppn == ExtentMap[templpn-1].ppn + ExtentMap[templpn-1].num){
				// merge...
				mergenum = ExtentMap[templpn-1].num;
				mergelpn = ExtentMap[templpn-1].lpn;
					
				for(i=0 ; i< count + mergenum ; i++){
					ExtentMap[i+mergelpn].ppn = ExtentMap[templpn-1].ppn;
					ExtentMap[i+mergelpn].lpn = ExtentMap[templpn-1].lpn;
					ExtentMap[i+mergelpn].num = count + mergenum;
				}
				//insert record
				
				fprintf(wfp, "insert\n%d %d\n", mergelpn, ExtentMap[templpn-1].ppn);
				MERGEBUF_count();
			}else{
				for(i=0 ; i<count ;i++){
					ExtentMap[i+templpn].ppn = startppn;
					ExtentMap[i+templpn].lpn = templpn;
					ExtentMap[i+templpn].num = count;
				}
				fprintf(wfp, "insert\n%d %d\n", templpn, startppn);

				//insert record
			}
			templpn += count;
		}
	}while(count < num);
}

void M_write(int lpn, int num){

//	if(countNum.write %1000 == 0)
//		block_debug();
	//printf("WRITE START\n");
	//block_debug();
	int tempLpn, i, count, from, to, last;
	int oldppn, startlpn, block_num, offset;
	_Extent tempExtent;
	int nlevelmerge =0 ;
	
	last = lpn+num;
	tempLpn = lpn;

	while( lpn <= tempLpn && tempLpn < last ){
		tempExtent = ExtentMap[tempLpn];
		if(tempExtent.lpn == -1){	
			// first time write
			count = 0;
			startlpn = tempLpn;

			while(ExtentMap[tempLpn].lpn == -1 && tempLpn < last ){
				count ++;
				tempLpn ++;
			}

			//allocate new page
			allocate_page(startlpn, count);
			num -= count;	
		}else{
			// old ppn and old lpn
			oldppn = tempExtent.ppn;
			startlpn = tempExtent.lpn;

			// from where to update?
			from = (tempLpn - startlpn) ;

			// to where to update?
			if(tempLpn + num < startlpn + tempExtent.num){
				to = tempLpn + num - startlpn;
			}else{ 
				to = tempExtent.num; 
			}

			// invalidate page
			for(i= from ; i<to ; i++){
				block_num = (oldppn+i) / PAGE_PER_BLOCK;
				offset = (oldppn+i) % PAGE_PER_BLOCK;
				if(PBinv[block_num] == 128){
					printf("invalid!!\n");
				}
				PB[block_num].valid[offset] = 0;
				PBinv[block_num]++;
			}

			/// extent will be splitted   !! //////////
			if(from != 0 && to < tempExtent.num) {
				for(i=0 ; i<from ; i++) {
					ExtentMap[startlpn+i].num = from;
				}
				fprintf(wfp, "insert\n%d %d\n", startlpn, oldppn);
				//insert record
				for(i=to; i<tempExtent.num; i++) {
					ExtentMap[startlpn+i].lpn = startlpn+to;
					ExtentMap[startlpn+i].ppn = oldppn + to;
					ExtentMap[startlpn+i].num = tempExtent.num-to;
				}
				fprintf(wfp, "insert\n%d %d\n", startlpn+to, oldppn+to);
				//insert record
				SPLITBUF_count();

			}else if(from != 0){
				for(i=0 ; i<from ; i++){
					ExtentMap[startlpn+i].num = from;
				}
				fprintf(wfp, "insert\n%d %d\n", startlpn, oldppn);
				//insert record
				SPLITBUF_count();
				
			}else if(to < tempExtent.num){
				for(i=to ; i<tempExtent.num ; i++){
					ExtentMap[startlpn+i].ppn = oldppn + to;
					ExtentMap[startlpn+i].lpn = startlpn + to;
					ExtentMap[startlpn+i].num = tempExtent.num - to;
				}
				//insert record
				fprintf(wfp, "insert\n%d %d\n", startlpn+to, oldppn+to);
				//update splited blockint 
				SPLITBUF_count();
				
			}

			allocate_page( from+startlpn, to-from);

			tempLpn =  startlpn + to;
			num = num - (to - from);
		}
	}

	if(FREE_BLOCK <= 20 ){
		M_GC();	
	}
	//printf("WRITE END : ");
	//block_debug();
}

void M_GC(){
	int victimIndex, i, num, j;
	_Extent victimExtent;

	num = 0;

	
		
		victimIndex = find_victim();
		FREE_BLOCK ++;
		BLOCKGC_count();

		if(PBinv[victimIndex] == PAGE_PER_BLOCK){
			// when there is any valid page
			PBinv[victimIndex] = -1;
			for(i=0 ; i<PAGE_PER_BLOCK ; i++){
				PB[victimIndex].valid[i] = -1;
			}
		}else if(PBinv[victimIndex] > PAGE_PER_BLOCK){
			printf("invalid!! %d\n", PBinv[victimIndex]);
		}else{
			// when there is some valid pages
			int extent_copy = 0;
			int real_copy = 0;
			for(i=0 ; i<PAGE_PER_BLOCK ;i++){
				if( PB[victimIndex].valid[i] == 1){
					victimExtent = ExtentMap[PB[victimIndex].PPN2LPN[i]] ;
					
					allocate_page(victimExtent.lpn, victimExtent.num);
					BLOCKGC_WRITE_count(victimExtent.num);
					BLOCKGC_READ_count(victimExtent.num);
				
					int LPN_debug = PB[victimIndex].PPN2LPN[i];
					
					if(victimExtent.lpn > LPN_debug || victimExtent.num+victimExtent.lpn<LPN_debug){
						printf("ERROR, WRONG\n");
					}			


					for(j=0 ; j<victimExtent.num ; j++){
						if(PB[victimIndex].valid[i+j]==1){
							real_copy++;
						}
						PB[victimIndex].valid[i+j] = -1;
					}
					extent_copy = extent_copy + victimExtent.num;
					i += (victimExtent.num - 1);
				}else{
					PB[victimIndex].valid[i] = -1;
				}
				
			}
			int j;
			for(j=0;j<PAGE_PER_BLOCK;j++){
				if(PB[victimIndex].valid[j] ==1){
					real_copy++;
				}
			}
			
			if(extent_copy != 128 - PBinv[victimIndex]){
				printf("ERROR : e %d, i %d\n", extent_copy, PBinv[victimIndex]);
			}
			if(128-real_copy != PBinv[victimIndex]){
				printf("ERROR : r%d e%d\n", 128 - real_copy, PBinv[victimIndex]);
			}
			PBinv[victimIndex] = -1;
		}

	
}
