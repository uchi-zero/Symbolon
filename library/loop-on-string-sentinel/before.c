#include <stdio.h>
int main(void){
  int n,i,h=0,t=0;
  char s[101];
  scanf("%d\n%s",&n,s);
  for (i=0;i<n;i++){
    if (s[i]=='(') h++;
    else{
      if (h) h--;
      else t++;
    }
  }
  for (i=0;i<t;i++) printf("(");
  printf("%s",s);
  for (i=0;i<h;i++) printf(")");
  printf("\n");
  return 0;
}