#include<stdio.h>
int main(void)
{
    int n,a[11],i,j,w;
	char card[100];
	while(1){
	    scanf("%d",&n);
		if(n==0) break;
		scanf("%s",card);
		for(i=0;i<=n;i++) a[i]=0;
		j=0;
		for(i=0;i<100;i++){
		    if(card[i]=='S'){
			    a[n]=a[n]+a[j]+1;
				a[j]=0;
			}
			if(card[i]=='L'){
			    a[j]=a[j]+a[n]+1;
				a[n]=0;
			}
			if(card[i]=='M'){
			    a[j]++;
			}
			j++;
			if(j==n) j=0;
		}
		for(i=0;i<n-1;i++){
		    for(j=i+1;j<n;j++){
			    if(a[i]>a[j]){
				    w=a[i];
					a[i]=a[j];
					a[j]=w;
				}
			}
		}
		for(i=0;i<=n;i++){
		    if(i==n) printf("%d\n",a[i]);
			else printf("%d ",a[i]);
		}
	}
	return 0;
}