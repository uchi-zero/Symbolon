#include<stdio.h>

int lcm(int a,int b){
    int m;
    int p=a*b;
    m = a%b;
    while(m>0){
        a = b;
        b = m;
        m = a%b;
    }
    return p/b;
}

int main(){
    int n,a,b;
    scanf("%d",&n);
    scanf("%d %d",&a,&b);
    a = lcm(a,b);
    while(n>2){
        scanf("%d",&b);
        a = lcm(a,b);
        n--;
    }
    printf("%d\n",a);
}