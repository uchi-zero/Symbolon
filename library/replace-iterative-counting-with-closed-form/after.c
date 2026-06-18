#include <stdio.h>

int main(void){
	int n;
	int ans;

	while(1){
		ans = 0;
		scanf("%d", &n);
		if(n == 0) break;

		while(1){
			ans = ans + n / 5;
			n = n / 5;
			if(n == 0) break;
		}

		printf("%d\n", ans);
	}

	return 0;
}