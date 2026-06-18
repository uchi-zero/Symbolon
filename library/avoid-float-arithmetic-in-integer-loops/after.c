#include <stdio.h>
long long n, m, i, j, k;
int main(){
	scanf("%lld", &m);
	n = 100;
	while(n < m){
		n = n + n/100;
		k++;
	}
	printf("%lld\n", k);
	return 0;
}