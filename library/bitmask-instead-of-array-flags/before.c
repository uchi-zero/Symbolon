#include <stdio.h>
int D[10];

int main(){
  int N,K,i,h,flag,temp;
  scanf("%d %d",&N,&K);
  for(i=0;i<K;i++){
    scanf("%d",&h);
    D[h]=1;
  }
  
  do{
    flag=0;
    temp=N;
    while(temp!=0){
      if(D[temp%10]==1){
          flag=1;
          break;
      }else{
          temp=temp/10;
      }
    }
    if(flag==1)
      N++;
  }while(flag!=0);
  
  
  printf("%d\n",N);
}