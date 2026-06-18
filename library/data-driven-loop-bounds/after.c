#include <stdio.h>
int main(void){
    char s[22]={'\n'},*p;
    gets(s+1);
    for(p=s;*++p;);
    for(p--;s<=p;p--)putchar(*p);
    return 0;
}