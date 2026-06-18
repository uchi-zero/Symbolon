// Your code here!
#include <stdio.h>
#include <math.h>
double a,b,c;
int main(){
scanf("%lf %lf",&a,&b);
c=floor(log10(b)+1);
c=a*pow(10,c)+b;
if(pow(floor(sqrt(c)),2)==c){
printf("Yes");
}else{
printf("No");
}
return 0;
}