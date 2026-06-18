#include <stdio.h>

int main(void){
		
	int a;
		
	while(1){
		if(scanf("%d", &a) == EOF )
				break;
			
		a = a % 39;
		if(a == 0)	a = 39;
		printf("3C%02d\n", a);
	}
		
	return 0;
}