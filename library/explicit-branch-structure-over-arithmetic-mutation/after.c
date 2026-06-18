#include<stdio.h>
void total();
int main()
{
    int t,i;
    scanf("%d",&t);
    for(i=1;i<=t;i++)
    {
        total();
    }
}
void total()
{
   long long int c1,c2,c3,a1,a2,a3,a4,a5;
   scanf("%lld %lld %lld",&c1,&c2,&c3);
   scanf("%lld %lld %lld ",&a1,&a2,&a3);
   scanf("%lld  %lld",&a4,&a5);
   if(c1>=a1&&c2>=a2&&c3>=a3)
   {
       if(a4<=(c1-a1)+(c3-a3))
        {
            if(a4-(c1-a1)>=0)
        {

            if(a5<=(c2-a2)+(c3-a3)-(a4-(c1-a1)))
                printf("YES\n");
             else printf("NO\n");

        }
        if(a4-(c1-a1)<0)
            if(a5<=(c2-a2)+(c3-a3))
                printf("YES\n");
            else printf("NO\n");
        }
        else printf("NO\n");
       //printf("c1=%lld,c3=%lld,s1=%lld,s2=%lld\n",c1,c3,s1,s2);


        }
        else
            printf("NO\n");

}