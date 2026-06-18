#include<stdio.h>
char p[110];
int main(){
    int i;
    scanf("%s",p);
    for(i=0;p[i];i++)if(p[i]=='C')break;
    for(i=i+1;p[i];i++)if(p[i]=='F')break;
    if(p[i]=='F')printf("Yes\n");
    else printf("No\n");
}