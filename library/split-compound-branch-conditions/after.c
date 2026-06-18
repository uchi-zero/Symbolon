#include<stdio.h>
#include<math.h>

int main(){
		double a,b,r1,d,e,r2;
		scanf("%lf%lf%lf%lf%lf%lf",&a,&b,&r1,&d,&e,&r2);

		double di=sqrt((a-d)*(a-d)+(b-e)*(b-e));

		if(di+r1<r2)printf("0\n");
		else if(di+r2<r1)printf("0\n");
		else if(di==r2+r1)printf("3\n");
		else if(di>r2+r1)printf("4\n");
    else if(di+r2==r1)printf("1\n");
    else if(di+r1==r2)printf("1\n");
    else printf("2\n");
}