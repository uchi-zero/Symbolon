#include<stdio.h>
int main(void) {  int qq, n, i, u;scanf("%d", &n);  for(i = 1;i <= 9;i++) {for(u = 1;u <= 9;u++) { qq = i * u; if(qq == n){puts("Yes"); return 0;}}}puts("No");return 0;}