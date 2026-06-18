#include <stdio.h>
 
void output(int* list,int listnum,int num,int max);
 
int main(void) {
    int list[30];
    int n;
    while(1) {
        scanf("%d",&n);
        if(n==0)break;
        output(list,0,n,n);
    }
    return 0;
}

void output(int* list,int listnum,int num,int max) {
    int i;
    if(num<=0) {
        printf("%d",list[0]);
        for(i=1;i<listnum;i++) {
            printf(" %d",list[i]);
        }
        puts("");
    } else {
        for(i=(max>num?num:max);i>0;i--) {
            list[listnum]=i;
            output(list,listnum+1,num-i,i);
        }
    }
}