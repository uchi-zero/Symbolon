#include<stdio.h>
int main(){
long n,a,b,ans;
scanf("%ld%ld%ld",&n,&a,&b);
if(n==1){
if(a==b)
ans=1;
else
ans=0;
}
else{
if(a>b)
ans=0;
else
ans=(n-2)*(b-a)+1;
}
printf("%ld\n",ans);
return 0;
}