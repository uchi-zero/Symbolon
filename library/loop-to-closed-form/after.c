#include<stdio.h>
#include<math.h>

int n;

int main(void){
	scanf("%d",&n);
	printf("%d\n",1<<((int)log2(n)));
	return 0;
}