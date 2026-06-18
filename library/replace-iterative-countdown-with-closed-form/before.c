#include<stdio.h>
int main(void)
{
	int a[101], temp = 0, ruio = 0;
	for (int i = 1; i <= 5; i++) {
		scanf("%d", &a[i]);
	}
	while (a[2] > 0 || a[3] > 0) {
		a[2]-=a[4];
		a[3] -= a[5];
		ruio++;
	}
	printf("%d\n", a[1]-ruio);
	return 0;
}