#include<stdio.h>
int main(void){
  char a[2];
  char b[]="ACGT";
  int i,j=0;
  scanf("%c",a);
  for(i=0;i<4;i++){
    if(a[0]==b[i]) j=i;
  }
  printf("%c\n",b[3-j]);
  return 0;
}