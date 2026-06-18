#include <stdio.h>

int main()
{
  int N,K,i;
  char S[10];

  scanf("%d",&N);
  scanf("%s",&S);
  scanf("%d",&K);

  for(i=0;i<N;i++)
  {
    if(S[i]!=S[K-1])
      S[i]='*';
  }

  printf("%s",S);

  return 0;
}