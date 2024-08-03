#!/bin/bash

# create
./doe_tool -o 0x12 -t 119 -d 32 -K 32 -V 8

# insert & query & delete
./doe_tool -o 0x32 -t 119 -k "00 aa bb cc dd ee ff 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff" -V 8
./doe_tool -o 0x30 -t 119 -k "00 aa bb cc dd ee ff 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff" -v "01 02 03 04 05 06 07 08"
./doe_tool -o 0x32 -t 119 -k "00 aa bb cc dd ee ff 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff" -V 8
./doe_tool -o 0x31 -t 119 -k "00 aa bb cc dd ee ff 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff"

# insert multi
./doe_tool -o 0x30 -t 119 -k "00 aa bb cc dd ee ff 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff" -v "00 f2 f3 f4 f5 f6 f7 f8"
./doe_tool -o 0x30 -t 119 -k "01 aa bb cc dd ee ff 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff" -v "01 f2 f3 f4 f5 f6 f7 f8"
./doe_tool -o 0x30 -t 119 -k "02 aa bb cc dd ee ff 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff" -v "02 f2 f3 f4 f5 f6 f7 f8"
./doe_tool -o 0x30 -t 119 -k "03 aa bb cc dd ee ff 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff" -v "03 f2 f3 f4 f5 f6 f7 f8"

./doe_tool -o 0x32 -t 119 -k "00 aa bb cc dd ee ff 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff" -V 8
./doe_tool -o 0x32 -t 119 -k "01 aa bb cc dd ee ff 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff" -V 8
./doe_tool -o 0x32 -t 119 -k "02 aa bb cc dd ee ff 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff" -V 8
./doe_tool -o 0x32 -t 119 -k "03 aa bb cc dd ee ff 34 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ff" -V 8

# batch query test
echo -ne "\\x00\\xaa\\xbb\\xcc\\xdd\\xee\\xff\\x34\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\xff" > key_list
echo -ne "\\x01\\xaa\\xbb\\xcc\\xdd\\xee\\xff\\x34\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\xff" >> key_list
echo -ne "\\x02\\xaa\\xbb\\xcc\\xdd\\xee\\xff\\x34\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\xff" >> key_list
echo -ne "\\x03\\xaa\\xbb\\xcc\\xdd\\xee\\xff\\x34\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\x00\\xff" >> key_list
./doe_tool -o 0x36 -t 119 -I key_list -O pair_list -e 40 -c 4

# delete
./doe_tool -o 0x13 -t 119
