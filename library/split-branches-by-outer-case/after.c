#include <stdio.h>
#include <math.h>

int main(int argc, const char * argv[]) {

    int c1x, c1y, c1r, c2x, c2y, c2r;
    scanf("%d %d %d", &c1x, &c1y, &c1r);
    scanf("%d %d %d", &c2x, &c2y, &c2r);
    
    if(c1r<c2r){
        if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) > c1r + c2r) printf("4\n");
        else if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) == c1r + c2r) printf("3\n");
        else if((c2r-c1r<sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2))) && (sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) < c1r + c2r)) printf("2\n");
        else if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) == c2r-c1r) printf("1\n");
        else if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) < c2r-c1r) printf("0\n");
        
        
    }
    
    else{
        if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) > c1r + c2r) printf("4\n");
        else if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) == c1r + c2r) printf("3\n");
        else if(((c1r-c2r<sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2))) && (sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) < c1r + c2r))) printf("2\n");
        else if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) == c1r-c2r) printf("1\n");
        else if(sqrt(pow(c1x-c2x,2)+pow(c1y-c2y,2)) < c1r-c2r) printf("0\n");
    }
}