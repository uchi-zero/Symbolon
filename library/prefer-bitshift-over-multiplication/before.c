#include<stdio.h>

int main(void){
  long long int X,Y;
  scanf("%lld",&X);
  scanf("%lld",&Y);
  int i=0;
  while(X<=Y){
    X *=2;
    i++;
  }
  printf("%d",i);
}