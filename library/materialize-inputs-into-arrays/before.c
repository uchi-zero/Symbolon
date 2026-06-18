#include<stdio.h>

int main(){

int x,y,n;
int X=0,Y=0;


scanf("%d",&n);

for(int i=0;i<n;i++)
{
scanf("%d %d",&x,&y);
if(i==0){Y=y;X=x;}
if(Y==y&&x<X){X=x;Y=y;}
else if(Y<y){X=x;Y=y;}

}

printf("%d %d\n",X,Y);
return 0;
}