#include<stdio.h>
 
int main(){
  int box[55555]={},a[55],i,j,n,cnt=0;
  scanf("%d",&n);
  for(i=2;i<55555;i++)box[i]=1;
  for(i=2;i<55555;i++){
    if(box[i]){
      if(i%10==1) a[cnt++]=i;
      for(j=i;j<55555;j+=i) box[j]=0;
    }
    if(cnt==n) break;
  }
  for(i=0;i<n;i++){
    printf("%d ",a[i]);
  }
  printf("\n");
}