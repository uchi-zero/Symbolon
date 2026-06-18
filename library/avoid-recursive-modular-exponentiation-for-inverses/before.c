#include<stdio.h>
int H,W,A,B,fac[222222];
#define mod 1000000007
int add(int x,int y){
	return (x+=y)<mod?x:x-mod;
}
int mul(int x,int y){
	return (int)((long long int)x*y%mod);
}
int pw(int x,int y){
	int r=1;
	while(1){
		if(y&1)r=mul(r,x);
		if(y>>=1)x=mul(x,x);else
			break;
	}
	return r;
}
int inv(int x){
	return pw(x,mod-2);
}
int c(int x,int y){
	--x;--y;
	return mul(fac[x+y],inv(mul(fac[x],fac[y])));
}
int main(){
	fac[0]=1;
	for(int i=1; i<222222; i++)
		fac[i] = mul(fac[i-1],i);
	scanf("%d%d%d%d",&H,&W,&A,&B);
	int res = 0;
	for(int i=B+1; i<=W; i++)
		res = add(res, mul(c(H-A,i),c(A, W-i+1)));
	printf("%d\n",res);
	return 0;
}