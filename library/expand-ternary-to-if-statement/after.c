#include<stdio.h>
int main(){
  int A,B,C;
  scanf("%d%d",&A,&B);
  C=A*B;
  if(A>=10||B>=10){
    C=-1;
  }
  printf("%d\n",C);
}