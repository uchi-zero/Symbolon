#include<stdio.h>
int main()
{
  int a,b,c;
  scanf("%d%d%d",&a,&b,&c);
  int r=0;
  for(;;)
  {
    if(a%2+b%2+c%2!=0)break;
    if(a==b&b==c)
    {
      printf("-1\n");
      return 0;
    }
    int s=a/2+b/2+c/2;
    a=s-a/2,b=s-b/2,c=s-c/2;
    r++;
  }
  printf("%d\n",r);
}