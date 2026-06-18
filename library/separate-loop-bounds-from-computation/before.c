#include<stdio.h>
int main(){
int n,i,j,ans=1;
scanf("%d",&n);
for(i=1;i<40;i++){
j=i;
if(i!=1){
while(j<=n){
j=j*i;
if(j<=n&&j>ans)
ans=j;
}
}
}
printf("%d\n",ans);
return 0;
}