#include<stdio.h>
char s[15];
int main(){
	int i,j,x,y;
	scanf("%s",s);
	for(i=0;i<8;i++){
		x=s[0]-'0';
		j=3;
		y=i;
		while(j--){
			if(y%2){
				x-=s[3-j]-'0';
			}else{
				x+=s[3-j]-'0';
			}
			y/=2;
		}
		if(x==7){
			y=i;
			printf("%c",s[0]);
			j=3;
			while(j--){
				if(y%2){
					printf("-%c",s[3-j]);
				}else{
					printf("+%c",s[3-j]);
				}
				y/=2;
			}
			printf("=7\n");
			return 0;
		}
	}
	return 0;
}