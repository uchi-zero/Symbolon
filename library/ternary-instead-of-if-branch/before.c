#include<stdio.h>
int main() {
	int n,A[25],B[25],i,ans=0;
	scanf("%d",&n);
	for(i=0;i<n;i++) scanf("%d",&A[i]);
	for(i=0;i<n;i++) scanf("%d",&B[i]);
	for(i=0;i<n;i++)
	  if(A[i]>B[i]) ans+=A[i]-B[i];
	printf("%d",ans);
	return 0;
}