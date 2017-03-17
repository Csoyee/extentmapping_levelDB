#define MIN_FREE_BLOCK 20
#define GC_DEFINE_VAL	21

int BLOCK_PER_FLASH;

typedef struct _Extent{
	int lpn;
	int ppn;
	int num;
}_Extent;

typedef struct update{
	int block_num;
	int offset; 
}update;

int *PBinv;
update udata;
char traceFileName[30];
  
void M_init();
void M_read(int lpn, int num);
void M_write(int lpn, int num);
void M_GC();
