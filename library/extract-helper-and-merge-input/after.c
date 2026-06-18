#include <stdio.h>
#define LL long long
LL myabs(LL a){
	return a>=0?a:-a;
}
int main(){
	LL x,k,d,ans;
	scanf("%lld%lld%lld",&x,&k,&d);
	x=myabs(x);
	if(x/d>=k)ans=x-k*d;
	else{//x/d<k
		k-=x/d;
		if(k%2==0)ans=x%d;
		else ans=d-x%d;
	}
	printf("%lld\n",ans);
	return 0;
}