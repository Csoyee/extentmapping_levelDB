#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "leveldb/db.h"
#include "leveldb/cache.h"

using namespace std;
leveldb::DB* db;
leveldb::WriteOptions writeOptions;
leveldb::ReadOptions readOptions;
leveldb::Iterator* it;

long long int BUFFER_SIZE;

    	int count=0;
int trace_res_parsing(FILE *fp){
	char str[100];
	char *op;
	char keybuf[30], valbuf[30];
	int key, value;
   	
    	char *read;
    	size_t read_len;
	fscanf(fp, "%s", str);

	if(strcmp("end", str)==0){
		printf("end of file %d\n", count);
		return 2;
	}else{
		count++;
		fscanf(fp,"%d", &key);
		fscanf(fp,"%d", &value);
		if(strcmp("insert", str)==0){
        		ostringstream keyStream;
        		keyStream << key;
                	ostringstream valueStream;
        		valueStream << value;
        		db->Put(writeOptions, keyStream.str(), valueStream.str());
  
		} else if(strcmp("read", str)==0){
    			ostringstream keyStream;
			keyStream << key;
			std::string result;
			leveldb::Status s = db->Get(readOptions, keyStream.str(), &result);
			if(s.IsNotFound()){
				printf("not found!!\n");
			}
		} else if(strcmp("start", str)==0){
			printf("aging Fin!!\n");
		}else{
			printf("invalid read!\n");
		}

		return 1;
	}
}

int main(int argc, char* argv[]){

	FILE *fp;
	char filename[40];
	int res = 0, readNum=0;
	char buf[1024];
	int i=0;


	printf("%d\n" ,getpid());
	if(argc < 2){
		printf("input filename\n");
		return 0;
	}
	strcpy(filename, argv[1]);

	fp = fopen(filename,"r");
	fscanf(fp, "%lld", &BUFFER_SIZE);

	leveldb::Options options;
	options.block_cache = leveldb::NewLRUCache(BUFFER_SIZE);
	options.create_if_missing = true;
    	leveldb::Status status = leveldb::DB::Open(options, "./testdb", &db);
	
    	if (false == status.ok())
    	{
        	cerr << "Unable to open/create test database './testdb'" << endl;
        	cerr << status.ToString() << endl;
        	return -1;
    	}
	it = db->NewIterator(leveldb::ReadOptions());
	while(res != 2){
		res = trace_res_parsing(fp);	
	}

	readNum = fread( buf, 1, 1024, fp);
	printf("%s\n", buf);
    	delete it;    
    	// Close the database
    	delete db;
	delete options.block_cache;
	// print map read, write count
	fclose(fp);
	return 0;
}
