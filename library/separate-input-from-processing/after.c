#include<stdio.h>
int main(){
	int n,i,a[20]={},v[20]={};
	int tmp;
	scanf("%d",&n);
	for(i=0;i<n;i++){
		
		scanf("%d%d",&a[i],&v[i]);
	}
	int j,h;
	for(j=0;j<n-1;j++){
		for(h=n-1;h>j;h--){
			
			if(v[h]>v[h-1]){
				tmp=v[h];
				v[h]=v[h-1];
				v[h-1]=tmp;
				tmp=a[h];
				a[h]=a[h-1];
				a[h-1]=tmp;
				
			}
		}
		
	}
	int max=v[0];
	for(i=1;i<n;i++){
		if(max==v[i]){
			if(a[0]>a[i]){
				tmp=a[0];
				a[0]=a[i];
				a[i]=tmp;
			}
		}
	}
	
	
	printf("%d %d\n",a[0],v[0]);
	return 0;
}