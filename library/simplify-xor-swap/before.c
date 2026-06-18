#include<stdio.h>
int main(){char str[5];scanf("%s",str);if(str[2]){str[0]^=str[2]^=str[0]^=str[2]
;}printf("%s",str);return 0;}