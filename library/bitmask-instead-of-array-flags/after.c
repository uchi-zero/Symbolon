#include <stdio.h>

int bit(int x)
{
	int r = 0;
	while (x){
		r |= 1 << (x % 10);
		x /= 10;
	}
	return r;
}

int main()
{
	int N,K,b = 0;
	scanf ("%d %d",&N,&K); while (K--){
		int x; scanf ("%d",&x); b |= 1 << x;
	}

	while (bit(N)&b)N++;
	printf ("%d\n",N);

	return 0;
}