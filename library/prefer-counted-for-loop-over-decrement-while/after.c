#include<stdio.h>
int main(void)
{
  int n,i,count=0,hight=0,h;
  
  scanf("%d",&n);
  for(i=0;i<n;i++){
    scanf("%d ",&h);
    if(hight<=h){
      hight=h;
      count++;
    }
  }
  printf("%d\n",count);
  return 0;
}