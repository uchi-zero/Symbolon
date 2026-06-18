#include <stdio.h>

int main () {
	int a;
	scanf("%d",&a);
	if ((a%100)/10==7 || a/100==7 || (a%10)==7) {
		printf("Yes");
	}
	else printf("No");
	
	return 0;
}