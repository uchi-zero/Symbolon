#include <stdio.h>

int main()
{
	int N, M, s[6], c[6];
	scanf ("%d %d", &N, &M);
	for (int i = 0; i < M; i++) scanf ("%d %d", &s[i], &c[i]);

	int p = 1;
	for (int i = 1; i < N; i++) p *= 10;
	for (int d = p - (N == 1); d < p * 10; d++){
		char S[10];
		sprintf(S, "%d", d);
		int g = 0;
		for (int k = 0; k < M; k++){
			if (S[s[k] - 1] == c[k] + '0') g++;
		}
		if (g == M){
			puts(S);
			return 0;
		}
	}
	puts("-1");
	return 0;
}