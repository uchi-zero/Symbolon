#include<stdio.h>

#define min(a, b) ((a) < (b)?(a):(b))

int ans[55], i, j, k, x;
int main(){
    for(i = 0;i <= 50;i++){
	ans[i] = 1 << 30;
    }
    for(i = 0;i <= 25;i++){
	for(j = 0;j <= 17;j++){
	    for(k = 0;k <= 10;k++){
		int p = i * 2 + j * 3 + k * 5;
		if(p > 50)continue;
		int price;
		price = (i / 5) * 4 * 380 + (i % 5) * 380;
		price += (j / 4) * 550 * 4 * 85 / 100 + (j % 4) * 550;
		price += (k / 3) * 3 * 850 * 88 / 100 + (k % 3) * 850;
		ans[p] = min(price, ans[p]);
	    }
	}
    }
    while(scanf("%d", &x), x){
	printf("%d\n", ans[x / 100]);
    }
    return 0;
}