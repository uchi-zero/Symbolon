#include <stdio.h>

int main(){
  int A,B;
  scanf("%d%d",&A,&B);
  if((A+B) % 2 == 1){
    printf("IMPOSSIBLE");
  }else{
    printf("%d",(A+B) / 2 );
  }
}