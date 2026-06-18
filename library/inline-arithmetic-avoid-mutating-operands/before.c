#include <stdio.h>
int main(){
int a,b,c;
  scanf("%d%d%d",&a,&b,&c);
  a-=c;
  b+=c;
  printf("%d\n",a/b);
}