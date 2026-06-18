#include<stdio.h>
int main(){
    char str[20]={0},i;
    scanf("%s",&str);
    for(i=19;i>=0;i--){
        if(str[i]!=0)printf("%c",str[i]);
    }
    printf("\n");
    return 0;
}