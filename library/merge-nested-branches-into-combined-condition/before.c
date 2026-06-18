#include<stdio.h>
int main(void)
{
	int m,f,b;
	int ans=0;

	scanf("%d",&m);
	scanf("%d",&f);
	scanf("%d",&b);

	if(m>=b){
		printf("0\n");
	}else{
		ans=b-m;
		if(f>=ans){
			printf("%d\n",ans);
		}else{
			printf("NA\n");
		}
	}
	return 0;
}