#include<stdio.h>
int main(){
    int d,n,t=1;
    scanf("%d%d",&d,&n);
    for(int i=0;i<d;i++)t*=100;
    if(n==100)n++;
    printf("%d\n",t*n);
    return 0;
}