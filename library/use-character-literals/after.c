#include<stdio.h>
int main()
{
  char c;
  c = getchar();
  if(c - 'A' <= 25)
    printf("A");
  else
    printf("a");
}