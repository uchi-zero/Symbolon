#include <stdio.h>
int main()
{
	int N, K;
	char S[11];
	scanf("%d%s%d", &N, S, &K);
	int i;
	for (i = 0; S[i] != '\0'; i++) {
		if (S[i] != S[K-1])S[i] = '*';
	}
	printf("%s\n", S);
	return 0;
}