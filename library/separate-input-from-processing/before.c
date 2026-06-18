#include<stdio.h>
int main(void)
{
	int n,a[21],b[101],i,z,zz,max;
	max=-1;
	scanf("%d",&n);
	for(i=0;i<n;i++){
		scanf("%d %d",&a[i],&b[i]);
		if(b[i]>max){
			max=b[i];
			z=a[i];
		}
		if(max==b[i]){
			if(z<a[i]){
			z=z;
			}
			else if(z>a[i]){
			z=a[i];
			}
		}
	}
	printf("%d %d\n",z,max);
	return 0;
}