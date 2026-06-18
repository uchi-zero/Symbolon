#include<stdio.h>

int main(void){
	int i,kai;

	while(scanf("%d",&i)!=EOF){
		i%=39;
		if(i==0)i=39;
		if(i>9)
			printf("3C%d\n",i);
		else
			printf("3C0%d\n",i);
	}
	return 0;
}