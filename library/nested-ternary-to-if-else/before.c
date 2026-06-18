#include <stdio.h>
 
int main( void )
{
	char s[1+1];
	scanf("%s", s ) ;
	printf( s[0] == 'A' ? "T\n" : (s[0] == 'C' ? "G\n" : (s[0] == 'G' ? "C\n" : "A\n"))) ;
	return 0 ;
}