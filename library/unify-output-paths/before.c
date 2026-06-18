#include <stdio.h>
int main(void){
  char c;
  scanf("%c",&c);
  if(c=='A')printf("T");
  else if(c=='T') printf("A");
  else if(c=='C')printf("G");
  else printf("C");
  return 0;
}