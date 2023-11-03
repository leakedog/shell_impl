#!/bin/sh

cd shell 
make 
cd ..
cd tests
./clean.sh && ./prepare.sh
./run_all.sh /mnt/c/Users/bogda/Desktop/srodowisko/so-shell-leakedog/shell/bin/mshell
cd ..
