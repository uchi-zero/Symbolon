#include<stdio.h>
int main(){
  int n,v[22],c[22];
  scanf("%d",&n);
  for(int i=0;i<n;i++)scanf("%d",&v[i]);
  for(int i=0;i<n;i++)scanf("%d",&c[i]);
  int ans=0;
  for(int i=0;i<n;i++)if(v[i]-c[i]>0)ans+=v[i]-c[i];
  printf("%d",ans);
}