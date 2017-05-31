# extentmapping_levelDB

1. extent.c, extent.h: extent based mapping FTL, 
- By implementing this fie you can get a trace file.
- This trace file has insertion, deletion and search info

2. awhole.cc, aging.cc
- by using trace file get block trace. 
- this use level DB for testing. (can use other key-value data structure or something else)

3. trace_parsing.c
- trace pasing file for page mapping.


Operation
- make -f Makefile_main (main.c, extent.c, random.c related headers)
- ./test [filename] -buf buffer size (default buffer size: 3.2MB)

==> you can get trace file name: tracefilenamestoragesize (e.g. mds_0.csv34359737368)


analysis trace file using level DB
- make -f Makefile_awhole
- ./awhole [tracefilename]

How to get block trace??
sudo blktrace -d /dev/sda -o - | blkparse -i --o [filename]
==> to parsing this block trace
    - gcc trace_parsing.c
    - /a.out [filename] [pid]
