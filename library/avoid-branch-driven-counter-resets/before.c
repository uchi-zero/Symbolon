#include <stdio.h>

char s[105];

int main(){
    int N;
    scanf("%d", &N);
    scanf("%s", &s[1]);
    int cnt = 0, fill = 0;
    for(int i=1; i<=N; i++){
        if(s[i] == '(') cnt++;
        else cnt--;
        if(cnt < 0){ fill++; cnt = 0; }
    }
    while(fill--) printf("(");
    printf("%s", &s[1]);
    while(cnt--) printf(")");
    return 0;
}