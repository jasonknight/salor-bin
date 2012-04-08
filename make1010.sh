#!/bin/sh
make clean
qmake
make
mv salor salor1010
ncftpput -u salordemo-www -p hkKOiNlr demo.salorpos.com salor/public salor1010
