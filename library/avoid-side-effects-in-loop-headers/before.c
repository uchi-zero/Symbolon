#include<stdio.h>
int main(){
  int H,W,h,w,i=1,j;
  scanf("%d%d%d%d",&H,&W,&h,&w);
  if(H%h||W%w){
    for(puts("Yes");i<=H;++i,puts("")) for(j=0;j<W;++j,printf("%d ",(i%h||j%w)?1000:-1000*(h*w-1)-1));
  }else puts("No");
  return 0;
}