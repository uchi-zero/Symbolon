#include <stdio.h>
int main(void) {
 long long int i=0,a,X,Y;
scanf("%ld",&X);
scanf("%ld",&Y);
a=X;
while(a<=Y){
 a=a*2;
 i++;
}
printf("%d",i);
 return 0;
}