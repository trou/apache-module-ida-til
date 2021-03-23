#!/bin/bash

export NLSPATH=$HOME/bin/ida76
TILIB=/home/raph/tools/reverse/IDA/v7.6/tools/tilib76/

gcc -Iapache2-2.4.46/os/unix/ -Iapache2-2.4.46/include/ -Iapr-1.7.0/include/ -Iapr-util-1.6.1/include/ -E apr_all.h | grep -v "^# " > apr-1.7-all.h
 
$TILIB/linux/tilib @gcc64.cfg \
        -c \
        -b$NLSPATH/til/pc/gnulnx_x64.til \
        -hapr-1.7-all.h \
        -D__extension__= \
        "-D__builtin_va_list=void *" \
        apr-17-debian64.til | grep -v "Unrecognized attribute"
