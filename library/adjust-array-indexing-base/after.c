#include <stdio.h>
int main(void){
  int a,b,g[12]={1,3,1,2,1,2,1,1,2,1,2,1};
  scanf("%d%d",&a,&b);
  printf("%s\n",(g[a-1]==g[b-1])? "Yes":"No");
  return 0;
}