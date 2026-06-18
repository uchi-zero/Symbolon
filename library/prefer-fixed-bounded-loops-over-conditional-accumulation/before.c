#include <stdio.h>

long long calc(long long x,long long y){
	if(y == 0){
		return x;
	}else{
		return calc(y,x%y);
	}
}

int main(){
	int n;
	long long tmp,number,r_1,r_2;

	scanf("%d",&n);

	for(int i=0; i < n; i++){
		scanf("%lld",&tmp);
		if(i == 0){
			number = tmp;
		}else{
			if(number >= tmp){
				r_1 = calc(number,tmp);
				number = (number*tmp)/r_1;
			}else{
				r_1 = calc(tmp,number);
				number = (number*tmp)/r_1;
			}
		}
	}

	printf("%lld\n",number);
}