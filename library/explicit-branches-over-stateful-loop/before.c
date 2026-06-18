#include<stdio.h>
int main(void)
{
  int n,a,b,c,r[15],s;
  scanf("%d",&n);
  while(n--){
    scanf("%d%d",&a,&b);
    c=0;
    if(a>b)s=-1;
    else s=1;
    while(a!=b){
      r[c++]=a;
      if(a<6){
        a+=s;if(a==0)s=1;
      }
      else{
        a++;if(a>9)a=5,s=-1;
      }
    }
    for(a=0;a<c;a++){
      printf("%d ",r[a]);
    }
    printf("%d\n",b);
  }
  return 0;
}