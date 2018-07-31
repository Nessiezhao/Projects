#include <stdio.h>
#include<sys/time.h>
int main()
{
    gettimeofday();//线程安全的
    return 0;
}
