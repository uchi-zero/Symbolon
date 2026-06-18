#include <stdio.h> 

char w[110];
int cnt[30];
int check;
int main(){ 
	scanf("%s",w);
	for(int i=0;w[i]!='\0';i++){
		cnt[w[i]]++;
	}
	for(int i='a';i<='z';i++){
		if(cnt[i]%2)
			check=1;
	}
	if(check)
		printf("No");
	else
		printf("Yes");
}