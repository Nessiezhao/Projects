#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#define MAX 1024
//firstdata=100&lastdata=200
void myCal(char* buf)
{
    int x = 0;
    int y = 0;
    //把一个字符串格式化输出到某个变量中
    sscanf(buf, "firstdata=%d&lastdata=%d", &x, &y);

    printf("<html>\n");
    printf("<boday>\n");
    printf("<h3>%d + %d = %d</h3>\n", x, y, x + y);
    printf("<h3>%d - %d = %d</h3>\n", x, y, x - y);
    printf("<h3>%d * %d = %d</h3>\n", x, y, x * y);
    if(y == 0)
    {
        printf("<h3>%d / %d = %d, %s</h3>\n", x, y, -1, "(zero)");
        printf("<h3>%d % %%d = %d, %s</h3>\n", x, y, -1, "(zero)");
    }
    printf("<h3>%d / %d = %d</h3>\n", x, y, x / y);
    printf("<h3>%d %% %d = %d</h3>\n", x, y, x % y);
    printf("</boday>\n");
    printf("</html>\n");
}

int main()
{
    char buf[MAX] = {0};
    if(getenv("METHOD"))
    {
        //说明环境变量获取成功
        if(strcasecmp(getenv("METHOD"), "GET") == 0)
        {
            //说明是GET方法
            strcpy(buf,getenv("QUERY_STRING"));
        }
        else
        {
            //说明是post方法
            int content_length = atoi(getenv("CONTENT_LENGTH"));
            int i = 0;
            char c;
            for(; i < content_length; i++)
            {
                read(0, &c, 1);
                buf[i] = c;
            }
            buf[i] = '\0';
        }
    }

    myCal(buf);
    return 0;
}
