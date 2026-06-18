#include<stdio.h>
int A[12];
int main()
{
int i,a,b,c=0,d=0,n,e=0,f=0,l=-1,j;
char S[102];
	scanf("%d",&n);
while(n!=0){
	for(i=0;i<10;i++){
	A[i]=0;
	}
	scanf("%s",&S);
	for(i=0;i<100;i++){
	l++;
	if(l==n){
	l=0;
	}
	if(S[i]=='M'){
	A[l]++;
	}
	else if(S[i]=='S') {
	f=f+A[l]+1;
	A[l]=0;
	}
	else if(S[i]=='L'){
	A[l]=A[l]+(f+1);
	f=0;
	}
	}
	l=-1;
	for(i=0;i<n;i++){
		for(j=n-1;j>=i;j--){
		if(A[i]>A[j]){
		c=A[i];
		A[i]=A[j];
		A[j]=c;
		}
		
		}
	}
	for(i=0;i<n;i++){
	printf("%d ",A[i]);
	}
	printf("%d\n",f);
	f=0;
	scanf("%d",&n);
	}
	return 0;
	
}