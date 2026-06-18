#include <stdio.h>

int main(void)
{
	int i,total=0,max=0;
	char s[4];
	scanf("%s",s);
	for(i=0;i<3;i++){
		if(s[i]=='R')total++;
		else{
			if(total>=max)max=total;
			total=0;
		}
	}
	if(total>max)max=total;
	printf("%d",max);
	return 0;
}