#include <stdio.h>

int main()
{
    int s;
    scanf("%d",&s);
    for(int i=1;;i++)
    {
        if(s==1||s==2||s==4)
        {
            printf("%d\n",i+3);
            return 0;
        }
        if(s%2)s=3*s+1;
        else s=s/2;
    }
}