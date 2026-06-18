#include <stdio.h>
int main(){
  long long int a,b,c;
  scanf("%lld%lld%lld", &a, &b, &c);
  if(4*a*b < (c-a-b)*(c-a-b) && c-a-b > 0) {
    puts("Yes");
  }
  else {
    puts("No");
  }
}