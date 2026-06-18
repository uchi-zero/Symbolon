#include <stdio.h>
int arr[4];
int main(){
	for(int i=0;i<4;i++) scanf("%1d",arr+i);
	int a=arr[0]*10+arr[1], b=arr[2]*10+arr[3];
	int s=0;
	if(a<=12&&a>0) s++;
	if(b<=12&&b>0) s+=2;
	if(s==0) printf("NA");
	else if(s==3) printf("AMBIGUOUS");
	else if(s==1) printf("MMYY");
	else printf("YYMM");
}