#include<stdio.h>
typedef long long ll;
ll s,x,y;
int main(){
    scanf("%lld%lld",&x,&y);
    for(s=0;x<=y;s++){
        x<<=1;
    }
    printf("%lld\n",s);
}