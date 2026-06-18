#include <stdio.h>
int main(void){
  char a,b;
  scanf("%c %c",&a,&b);
  printf("%c\n",((a=='H'&&b=='H')||(a=='D'&&b=='D'))? 'H':'D');
  return 0;
}