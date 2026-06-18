#include <stdio.h>

int a,b,d,r;
double f;

int main(){
  scanf("%d %d",&a,&b);
  d=a/b;
  r=a%b;
  f=(double)a/b;
  printf("%d %d %lf\n",d,r,f);
}