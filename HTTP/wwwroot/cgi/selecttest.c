#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<mysql.h>
void test()
{
    
}
int main()
{
    char data[1024];
    if(getenv("METHOD"))
    {
        if(strcasecmp("GET",getenv("METHOD")) == 0)
        {
            strcpy(data,getenv("QUERY_STRING"));
        }
        else
        {
            //POST
            int content_length = atoi(getenv("CONTENT_LENGTH"));
            int i = 0;
            for(;i < content_length;i++)
            {
                read(0,data+i,1);
            }
        }
    }
    //printf("arg:%s\n",data);
    test();
    return 0;
}
