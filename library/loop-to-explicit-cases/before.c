#include <stdio.h>

int main(){
  int N;
  scanf("%d",&N);
  for(int i=0;i<100;i++){
    for(int j=0;j<100;j++){
      if(4*i+7*j==N) {printf("Yes"); return 0;}
    }
  }
  printf("No");
}