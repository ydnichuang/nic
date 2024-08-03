#!/bin/bash

# create
./doe_tool -o 0x10 -t 127 -d 32 -V 64

# store init value
./doe_tool -o 0x21 -t 127 -i 0 -v "11 22 33 44 55 66 77 88 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 88 77 66 55 44 33 22 11"
./doe_tool -o 0x21 -t 127 -i 2 -v "aa aa aa aa aa aa aa aa 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 88 77 66 55 44 33 22 11"
./doe_tool -o 0x21 -t 127 -i 4 -v "bb bb bb bb bb bb bb bb 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 88 77 66 55 44 33 22 11"
./doe_tool -o 0x21 -t 127 -i 6 -v "cc cc cc cc cc cc cc cc 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 88 77 66 55 44 33 22 11"

# read clear test
./doe_tool -o 0x20 -t 127 -i 0 -V 64
./doe_tool -o 0x26 -t 127 -i 0 -V 64

# single load test
./doe_tool -o 0x20 -t 127 -i 0 -V 64
./doe_tool -o 0x20 -t 127 -i 2 -V 64
./doe_tool -o 0x20 -t 127 -i 4 -V 64
./doe_tool -o 0x20 -t 127 -i 6 -V 64

echo -ne "\\x00\\x00\\x00\\x00\\x02\\x00\\x00\\x00\\x04\\x00\\x00\\x00\\x06\\x00\\x00\\x00" > index_list
# batch load test
./doe_tool -o 0x24 -t 127 -I index_list -O pair_list -e 68 -c 4

# batch read clear test
./doe_tool -o 0x2a -t 127 -I index_list -O pair_list -e 68 -c 4

# batch store test
./doe_tool -o 0x25 -t 127 -I pair_list -c 4
./doe_tool -o 0x20 -t 127 -i 0 -V 64
./doe_tool -o 0x20 -t 127 -i 2 -V 64
./doe_tool -o 0x20 -t 127 -i 4 -V 64
./doe_tool -o 0x20 -t 127 -i 6 -V 64

# delete table
./doe_tool -o 0x11 -t 127
