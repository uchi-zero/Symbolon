#include<stdio.h>
int main()
{
    int a,b[3],c[5],i,j,k,t;
    scanf("%d",&a);
    for(i=1;i<=a;i++)
    {
         for(j=0;j<=2;j++)
               scanf("%d",&b[j]);

           for(j=0;j<=4;j++)
                scanf("%d",&c[j]);

           for(j=0;j<=2;j++)
             b[j]=b[j]-c[j];



           if(c[3]>b[0])
              c[3]=c[3]-b[0];
           else if(c[3]<=b[0])
              c[3]=0;

               if(c[4]>b[1])
                  c[4]=c[4]-b[1];

              else if(c[4]<=b[1])
                   c[4]=0;


              b[2]=b[2]-c[3]-c[4];
            if((b[0]>=0)&&(b[1]>=0)&&(b[2]>=0))
                   printf("YES\n");
            else
                printf("NO\n");
        }
       return 0;
}