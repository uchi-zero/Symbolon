#include <stdio.h>

int main()
{
  char  s[101], tmp;
  int N, r=0, b=0;
  scanf("%d\n",&N);
  for(int i=0; i<N; i++){
    scanf("%c", &tmp);
    if(tmp=='R')r++;
    if(tmp=='B')b++;
  }
  if(r>b)printf("Yes\n");
  else printf("No\n");
}