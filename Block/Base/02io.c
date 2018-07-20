#include <stdio.h>
#include<unistd.h>
#include<termios.h>
int main()
{
    int c;

    struct termios ts;
    tcgetattr(0,&ts);
    ts.c_lflag &= ~ICANON;
    ts.c_lflag &= ~ECHO;
    tcsetattr(0,TCSANOW,&ts);
    char buf[1024] = {};
    int i = 0;
    while((c=getchar()) != '\n')
    {
        buf[i++] = c;
        putchar('*');
    }
    ts.c_lflag |= ICANON;
    ts.c_lflag |= ECHO;
    tcsetattr(0,TCSANOW,&ts);
    printf("buf=[%s]\n",buf);
    return 0;
}
