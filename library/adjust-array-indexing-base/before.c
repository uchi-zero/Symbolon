#include <stdio.h>

int main(int argc, char *argv[]){
  int ary[]={0,0,2,0,1,0,1,0,0,1,0,1,0};
  
  int x, y;
  scanf("%d %d", &x, &y);
  
  printf("%s", ary[x]==ary[y]?"Yes":"No");
  
  return 0;
}