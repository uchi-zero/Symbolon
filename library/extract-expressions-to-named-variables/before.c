#include <stdio.h>

int main(){
int a,b;
double c=0.0;
scanf("%d %d",&a,&b);
c=(double)a/(double)b;
printf("%d %d %f\n",a/b,a%b,c);
return 0;
}