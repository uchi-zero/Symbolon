#include <stdio.h>
int main(void){
  int a,b,d,t,i=0;
  scanf("%d%d",&a,&b);
  if (b==100) d=1000;
  else if (b<10) d=10;
  else d=100;
  t=a*d+b;
  while (i*i<t) i++;
  printf("%s\n",(i*i==t)? "Yes":"No");
  return 0;
}