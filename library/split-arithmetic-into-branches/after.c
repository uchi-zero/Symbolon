#include<stdio.h>
int main(){
  long long s,t,u;
  scanf("%lld",&s);
  t=s/1000000000+1;
  u=1000000000-s%1000000000;
  if(s%1000000000==0){
    t--;
    u=0;
  }
  printf("0 0 %lld 1000000000 %lld 1",u,t);
}