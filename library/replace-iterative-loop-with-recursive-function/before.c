#include <stdio.h>
int main(void){
  long int n,x,a,b,ans=0,t;
  scanf("%ld%ld",&n,&x);
  a=(x*2<n)? x:n-x;
  b=n-a;
  while (a){
    t=b%a;
    ans+=b-t;
    b=a;
    a=t;
  }
  printf("%ld",ans*3);

  return 0;
}