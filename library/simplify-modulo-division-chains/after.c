#include <stdio.h>
int main (){
	int n;
	scanf ("%d",&n);
	if (n%10==7||(n/10)%10==7||n/100==7){
		printf ("Yes\n");
	}else{
		printf("No\n");
	}
}