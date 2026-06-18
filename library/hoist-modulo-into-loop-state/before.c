#include <stdio.h>
int main()
{
	int n,m;
	while(scanf("%d%d", &n, &m)!=EOF){
		for(int i = m/n; i >= 1; i--){
			if(m % i == 0) {
				printf("%d\n",i); 
				break;
			}
		}
	}
}