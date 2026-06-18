#include<stdio.h>
int main(){
	int x1, y1, x2, y2,Xj,Yj;
	scanf("%d %d %d %d", &x1, &y1, &x2, &y2);
	Xj = x2 - x1;
	Yj = y2 - y1;
	printf("%d %d %d %d\n",x2-Yj,y2+Xj,x1-Yj,y1+Xj);
	return 0;
}