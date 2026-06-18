#include <stdio.h>
int main(){
	int N, M;
	scanf("%d %d", &N, &M);
	int estGcd = M / N;
	int residue = M % N;
	while((estGcd) && (residue % estGcd)){
		estGcd--;
		residue += N;
	}
	printf("%d\n", estGcd);
	return 0;
}