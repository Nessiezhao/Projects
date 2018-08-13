#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<mysql.h>

void InsertData(char* name,char* sex,char* phone)
{
    MYSQL* mysql_fd = mysql_init(NULL);//获取mysql文件描述符
    if(mysql_fd == NULL)
    {
        printf("mysql_init\n");
        return;
    }
    if(mysql_real_connect(mysql_fd,"127.0.0.1","root","root","student",3306,NULL,0) == NULL)//连接数据库
    {
        printf("connect failed!\n");//数据库连接失败
        return;
    }
    printf("connect mysql success!\n");
    //下发sql命令
    char sql[1024];
    sprintf(sql,"insert into student_info(name,sex,phone)values(\"%s\",\"%s\",\"%s\")",name,sex,phone);
    printf("sql = %s\n",sql);
    mysql_query(mysql_fd,sql);
    printf("sql:%s\n",sql);
    mysql_close(mysql_fd);//关闭mysql文件描述符
}
//#define asd
#ifdef asd
int main(){
    printf("hello world!\n");
    return 0;
}
#else
int main()
{
    char data[1024];
    if(getenv("METHOD"))//没有的话返回NULL
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
               read(0,data + i,1);
            }
        }
    }
    printf("arg:%s\n",data);
    char* name;
    char* sex;
    char* phone;
    strtok(data,"=&");
    name = strtok(NULL,"=&");
    strtok(NULL,"=&");
    sex = strtok(NULL,"=&");
    strtok(NULL,"=&");
    phone = strtok(NULL,"=&");

    printf("name = %s,sex = %s,phone = %s\n",name,sex,phone);
    InsertData(name,sex,phone);
    return 0;
}
#endif
