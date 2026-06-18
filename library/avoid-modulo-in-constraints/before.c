#include <stdio.h>
int n;

int main()
{
    scanf("%d",&n);
    //printf("y");
    for (long long i=1;i<=3500;i++)
    for (long long j=i;j<=3500;j++)
    {
        //printf("%lld %lld %lld\n",i,j,(i*j*n)  );
        if ( (3*i*j>n*(i+j) ) && (i*j*n)%( 4*i*j-n*(i+j) )==0  )
        {
            printf("%lld %lld %lld\n",i,j,(i*j*n)/(4*i*j-n*(i+j) ) );
            return 0;
        }

    }
}