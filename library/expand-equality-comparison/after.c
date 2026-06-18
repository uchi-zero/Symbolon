#include <stdio.h>

int main() {
    char a, b;
    scanf("%c %c", &a, &b);
    printf("%c\n", ((a == 'H') ^ (b == 'H')) ? 'D' : 'H');
}