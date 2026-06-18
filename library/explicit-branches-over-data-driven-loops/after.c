#include<stdio.h>

int main(){

int n,s,f;
int w;

//while(1){
scanf("%d",&n);

for(int i=0;i<n;i++){
scanf("%d %d",&s,&f);

if(s<=5){
 if(s<f){
for(int i=s;i<=f;i++)
{
if(i!=s)printf(" ");
printf("%d",i);
}
     }
else if(s>f){
for(int i=s;i>=f;i--)
{
if(i!=s)printf(" ");
printf("%d",i);
}
}
}
if(s>5){
 if(f>s){
for(int i=s;i<=f;i++)
{
if(i!=s)printf(" ");
printf("%d",i);
}
}
 else if(5<f&&f<s){
	 for(int i=s;i<=9;i++)
{
if(i!=s)printf(" ");
printf("%d",i);
}

for(int i=5;i>=0;i--)
{
printf(" ");
printf("%d",i);
}

for(int i=1;i<=f;i++)
{
printf(" ");
printf("%d",i);
}

}
 else if(f<=5){
for(int i=s;i<=9;i++)
{
if(i!=s)printf(" ");
printf("%d",i);
}

for(int i=5;i>=f;i--)
{
printf(" ");
printf("%d",i);
}

}
}printf("\n");
}

return 0;
}