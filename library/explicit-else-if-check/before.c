#include<stdio.h>
int main(){
char S[5];
int a=0,i;
scanf("%s",S);
for(i=0;i<4;i++){
if(S[i]=='+'){a++;}
else{a--;}
}
printf("%d",a);
return 0;


}