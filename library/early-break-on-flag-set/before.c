#include <stdio.h>

int main () {
    int i,j,m = 0;
    int N,M,K;
    scanf("%d %d %d",&N,&M,&K);
    for (i=0;i<=N;i++){
        for (j=0;j<=M;j++){
            if (j*(N - i)+i*(M - j) == K){m=1;}
        }
    }
    if (m==1){printf("Yes\n");}
    else{printf("No\n");}
    return 0;
}