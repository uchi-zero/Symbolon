#include<stdio.h>
int main()
{
    int a,b,c,x,i,j,k,s=0;
    scanf("%d\n%d\n%d\n%d",&a,&b,&c,&x);
    for(i=0;i<=a;i++)
        for(j=0;j<=b;j++)
        for(k=0;k<=c;k++)
    {

            if(i*500+j*100+k*50==x)s++;
    }
    printf("%d\n",s);
}