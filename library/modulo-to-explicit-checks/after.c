#include<stdio.h>
int main(void){
  int a,b;
  scanf("%d%d",&a,&b);
  if(a==2||b==2)
    printf("No");
  else
    printf("Yes");
  return 0;
}