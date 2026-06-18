#include<stdio.h>

char str[8];
int a,b,c,d;

int main(void){
	register int i,j,k,ans;
	gets(str);
	a=str[0]-'0';b=str[1]-'0';c=str[2]-'0';d=str[3]-'0';
	for(i='+';i<='-';i+=2)
		for(j='+';j<='-';j+=2)
			for(k='+';k<='-';k+=2){
				ans=a;
				if(i=='+')
					ans+=b;
				else
					ans-=b;
				if(j=='+')
					ans+=c;
				else
					ans-=c;
				if(k=='+')
					ans+=d;
				else
					ans-=d;
				if(ans==7)
					return printf("%d%c%d%c%d%c%d=7\n",a,i,b,j,c,k,d),0;
			}
	return 0;
}