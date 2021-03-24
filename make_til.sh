#!/bin/bash

export NLSPATH=$HOME/bin/ida76
TILIB=/home/raph/tools/reverse/IDA/v7.6/tools/tilib76/

ALL_HDR=apache2.2-all.h
gcc -Ihttpd-2.2.34/os/unix/ -Ihttpd-2.2.34/include/ -Ihttpd-2.2.34/srclib/apr/include/ -Ihttpd-2.2.34/srclib/apr-util/include/ -E apr_all.h | grep -v "^# " > $ALL_HDR

$TILIB/linux/tilib64 @gcc64.cfg \
        -c \
        "-b$NLSPATH/til/pc/gnulnx_x64.til" \
        "-h$ALL_HDR" \
        -D__extension__= \
        "-D__builtin_va_list=void *" \
        apache22-apr17-debian64.til | grep -v "Unrecognized attribute"
exit 0

gcc -Iapache2-2.4.46/os/unix/ -Iapache2-2.4.46/include/ -Iapr-1.7.0/include/ -Iapr-util-1.6.1/include/ -E apr_all.h | grep -v "^# " > apr-1.7-all.h
 
$TILIB/linux/tilib @gcc64.cfg \
        -c \
        -b$NLSPATH/til/pc/gnulnx_x64.til \
        -hapr-1.7-all.h \
        -D__extension__= \
        "-D__builtin_va_list=void *" \
        apr-17-debian64.til | grep -v "Unrecognized attribute"
