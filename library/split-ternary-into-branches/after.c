#include <stdio.h>

int main() {
long w=1;
long h=1;
scanf("%ld %ld",&w,&h);
if (w==1 || h==1)
printf("1");
else
printf("%ld",(w/2+w%2)*(h/2+h%2)+(w/2)*(h/2));
return 0;
}