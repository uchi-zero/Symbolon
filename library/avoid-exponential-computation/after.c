#include <stdio.h>
int main(){
    int a,b;
    scanf("%d%d",&a,&b);
    if(b==100)b++;
    printf("%d",b);
    for(int i=0;i<a*2;i++)printf("0");
    printf("\n");
}