#include <stdio.h>

int main(){
  char a[3];
  scanf("%s",a);
 
  if(a[2]=='\0'){
    printf("%s",a);
  }else{
    char u = a[0];
    a[0] = a[2];
    a[2] = u;
    printf("%s",a);
  }  
}