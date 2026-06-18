#include <stdio.h>

int main(void)
{
	long long int x,y=0,z,a,b,sum = 0;
	char c[101];
	for(b=0;b<100;b++) c[b] = '0';
	scanf("%s",c);
	scanf("%lld",&x);
	while(c[y] == '1') y++;
	if (y+1>x) printf("1");
	else printf("%c",c[y]);
	return 0;
}