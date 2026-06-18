#include <stdio.h>
int ax, ay, ar, bx, by, br;
int main() {
	scanf("%d%d%d%d%d%d", &ax, &ay, &ar, &bx, &by, &br);
	int norm = (ax - bx) * (ax - bx) + (ay - by) * (ay - by);
	if(norm > (ar + br) * (ar + br)) puts("4");
	else if(norm == (ar + br) * (ar + br)) puts("3");
	else if(norm > (ar - br) * (ar - br)) puts("2");
	else if(norm == (ar - br) * (ar - br)) puts("1");
	else puts("0");
}