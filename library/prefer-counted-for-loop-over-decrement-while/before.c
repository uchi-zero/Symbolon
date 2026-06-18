#include <stdio.h>
int main (){
	
	int N;
	int max = 0, H, count = 0;
	scanf("%d", &N);
	
	while(N--){
		scanf("%d", &H);
		if(H >= max ){
			count++;
			max = H;
		}
	}
	
	printf("%d", count);
	
	
	
	
	
	return 0;
}