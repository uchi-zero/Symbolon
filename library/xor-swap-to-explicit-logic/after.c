#include<stdio.h>
int main(){
  char s[4];
  scanf("%s",s);
  if(s[2]=='\0'){
    printf("%s",s);
  }else{
    int i;
    for(i=2;i>=0;i--){
      printf("%c",s[i]);
    }
  }
  return 0;
}