#!/bin/bash

export NLSPATH=$HOME/bin/ida76
TILIB=/home/raph/tools/reverse/IDA/v7.6/tools/tilib76/

ALL_HDR=apache2.2-all.h
HTTPD=httpd-2.2.34
gcc -I$HTTPD/os/unix/ -I$HTTPD/include/ -I$HTTPD/srclib/apr/include/ -I$HTTPD/srclib/apr-util/include/ -I$HTTPD/modules/filters/ -E apache_all.h | grep -v "^# " > $ALL_HDR

$TILIB/linux/tilib64 @gcc64.cfg \
        -c \
        "-b$NLSPATH/til/pc/gnulnx_x64.til" \
        "-h$ALL_HDR" \
        -D__extension__= \
        "-D__builtin_va_list=void *" \
        -t"Apache 2.2 - Linux 64bit" \
        apache22-debian64.til | grep -v "Unrecognized attribute"

ALL_HDR=apache2.4-all.h
gcc -DAPACHE24 -Ihttpd-2.4.46/os/unix/ -Ihttpd-2.4.46/include/ -Ihttpd-2.4.46/srclib/apr/include/ -Ihttpd-2.4.46/srclib/apr-util/include/ -E apache_all.h | grep -v "^# " > $ALL_HDR
 
$TILIB/linux/tilib64 @gcc64.cfg \
        -c \
        "-b$NLSPATH/til/pc/gnulnx_x64.til" \
        "-h$ALL_HDR" \
        -D__extension__= \
        "-D__builtin_va_list=void *" \
        apache24-apr17-debian64.til | grep -v "Unrecognized attribute"

cp apache24-apr17-debian64.til apache22-debian64.til "$NLSPATH/til/"
