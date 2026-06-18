#include<stdio.h>

int main(){

int L,A,B,C,D;
int t1,t2;
scanf("%d",&L);
scanf("%d",&A);
scanf("%d",&B);
scanf("%d",&C);
scanf("%d",&D);

t1=A/C;if(A%C!=0)t1++;
t2=B/D;if(B%D!=0)t2++;

if(t2>t1)t1=t2;

L=L-t1;
printf("%d\n",L);
return 0;
}