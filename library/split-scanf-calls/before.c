#include <stdio.h>
int main(){
	
	int K;
	int N;
	char S[52];
	
	scanf("%d %d %s", &N, &K, S);
	S[K-1] += 32;
	printf("%s", S);
	return 0;
}