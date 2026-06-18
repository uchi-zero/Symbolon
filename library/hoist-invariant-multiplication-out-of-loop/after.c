#include<stdio.h>
#include<math.h>
int main(){
int n,a,num=1,m;
	
	
	scanf("%d",&n);
	m=n;
	while(n--)
	{
		scanf("%d",&a);
		if(a%2==1)num*=1;
		else num*=2;
		
	}
	printf("%d",(int)pow(3,m)-num);
	
	
	
}