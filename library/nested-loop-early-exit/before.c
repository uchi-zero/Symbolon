#include<stdio.h>
int f(int a,int b){
int u,c=1;
for(u=0;u<b;u++)c*=a;
return c;
}
int main(){
int x,i,j,ans=1,y=0;
scanf("%d",&x);
for(i=2;i*i<=x;i++){
for(j=2;y<x;j++){
y=f(i,j);
if(y<=x&&y>ans)ans=y;
}
y=0;
}
printf("%d",ans);
return 0;

}