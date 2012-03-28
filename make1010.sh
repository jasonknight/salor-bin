#!/bin/sh
make clean
qmake
make
ncftpput -u salordemo-www -p hkKOiNlr demo.salorpos.com salor/public salor
