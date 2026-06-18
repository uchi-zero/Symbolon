#include <stdio.h>
int main(){

  int n,i=0,lo[52]={},cnt=0;
  long long r=1;
  char sa[53],sb[53];
  scanf("%d",&n);
  scanf("%s",sa);
  scanf("%s",sb);

  for(i=0;i<n;i++){
    if(sa[i]==sb[i]){
      lo[cnt]=1;
    }else{
      lo[cnt]=2;
      i++;
    }
    cnt++;
  }
  if(lo[0]==2) r*=6;
  else r*=3;
  //for(i=0;i<cnt;i++) printf("%d",lo[i]);
  for(i=1;i<cnt;i++){
    if(lo[i-1]==1&&lo[i]==1) r*=2;
    else if(lo[i-1]==1&&lo[i]==2) r*=2;
    else if(lo[i-1]==2&&lo[i]==2) r*=3;
    r%=1000000007;
  }
  printf("%lld\n",r);

  return 0;

}