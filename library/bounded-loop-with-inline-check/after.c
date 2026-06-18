#include<stdio.h>
int main(){
    long long k;
    char s[110];
    scanf("%s%lld",s,&k);
    for(int i=0;i<k;i++){
        if(s[i]!='1'){
            putchar(s[i]);
            return 0;
        }
    }
    puts("1");
    return 0;
}