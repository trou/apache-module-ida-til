#include <stdbool.h>
#include <stdio.h>

struct test {
    char a;
    int b;
    long c;
    long long d;
};
enum testenum {
    toto = 1
    };

int main()
{
    struct test a;
    enum testenum en;

    printf("sizeof(void *) (%lu)\n", sizeof(void *));
    printf("sizeof(char) (%lu)\n", sizeof(a.a));
    printf("int-char align (%lu)\n",(char *)&a.b-(char *)&a.a);
    printf("sizeof(int) (%lu)\n", sizeof(a.b));
    printf("long-int align (%lu)\n",(char *)&a.c-(char *)&a.b);
    printf("sizeof(long) (%lu)\n", sizeof(a.c));
    printf("long long-long align (%lu)\n",(char *)&a.d-(char *)&a.c);
    printf("sizeof(long long) (%lu)\n", sizeof(a.d));
    printf("sizeof(bool) %lu\n", sizeof(bool));
    printf("sizeof(short) %lu\n", sizeof(short));
    printf("sizeof(enum) %lu\n", sizeof(en));
}


