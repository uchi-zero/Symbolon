#include<stdio.h>
int main(){
	char a[110];
	int i, b[50]={},t=1;
	scanf("%s",a);
	for(i=0;a[i]!='\0';i++){
		b[a[i]-'a']++;
	}
	for(i=0;i<40;i++){
		if(b[i]%2!=0) t=0;
	}
	if(t==1)printf("Yes\n");
	else printf("No\n");
}