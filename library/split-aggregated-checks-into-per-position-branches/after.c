#include<stdio.h>
int main(){
int a,b,i,yj=0,x;
char s[12];
scanf("%d %d",&a,&b);
scanf("%s",s);
for(i=0;i<a;i++){
x=s[i]-'0';
if(x<0||9<x)yj++;
}
if(s[a]!='-')yj++;
for(i=a+1;i<a+b+1;i++){
x=s[i]-'0';
if(x<0||9<x)yj++;
}
if(yj==0)printf("Yes");
else printf("No");
return 0;
}