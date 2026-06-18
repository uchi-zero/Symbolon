#include<stdio.h>

int main(){

int x,y;

//while(scanf("%d %d",&x,&y)!=EOF){

for(int i=0;i<4;i++){
scanf("%d %d",&x,&y);

int t=0;
switch(x){
case 1:
t=6000;break;

case 2:
t=4000;break;

case 3:
t=3000;break;

case 4:
t=2000;break;
}

printf("%d\n",t*y);
}
return 0;
}