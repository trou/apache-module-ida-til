## Creating a TIL file for Apache

OS: Debian 64 sid (March 2021)


### Prereq

```
apt source libapr1
apt source apache2-dev
```
## TIL

### Compiler config

`-C` is *difficult*. Here's how to read the -`Cags44444` which you can find in tilib's `gcc.cfg`:

```
; from GCC 32 config:
; -Cags44444
; cmpgbeislLvB8 (expansion for for "Ca")
; us40144248i3n (default)
; gs44444
; |||||||||||||_ 8bytes scalars alignment
; ||||||||||||__ bitness
; |||||||||||___ calling convention
; ||||||||||____ sizeof(longlong)
; |||||||||_____ sizeof(long)    : 
; ||||||||______ sizeof(short) : 4
; |||||||_______ sizeof(int)   : 4
; ||||||________ sizeof(enum)  : 4
; |||||_________ sizeof(bool)  : 4
; ||||__________ defalign: 4
; |||___________ pointer size: 4
; ||____________ mem model: small
; |_____________ compiler: gcc
```

### Building TIL steps

* Use `sizes.c`
* Fill `gcc64.cfg`
* Make a top level header which includes everything
* Use `gcc -E`
* Fix warnings
