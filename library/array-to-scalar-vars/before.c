#include <stdio.h>
char a[5];
int main(){
	scanf("%s",a);if(a[0]==a[1]&&a[1]==a[2]&&a[0]==a[2]) puts("No");else puts("Yes");return 0;
}