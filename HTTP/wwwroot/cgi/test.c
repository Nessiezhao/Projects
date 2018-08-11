#include <stdio.h>
#include <mysql.h>

int main(){
    MYSQL* mysql_fd = mysql_init(NULL);//获取mysql文件描述符
    if(mysql_fd == NULL)
    {
        printf("mysql_init\n");
        return 0;
    }
    if(mysql_real_connect(mysql_fd,"127.0.0.1","root","root","student",3306,NULL,0) == NULL)//连接数据库
    {
        printf("connect failed!\n");//数据库连接失败
        return 0;
    }
    printf("connect mysql success!\n");
    //下发sql命令
    char sql[1024];
    sprintf(sql,"insert into student_info(name,sex,phone)values(\"%s\",\"%s\",\"%s\")","zhag","ss","ss");
    printf("sql = %s\n",sql);
    mysql_query(mysql_fd,sql);
    printf("sql:%s\n",sql);
    mysql_close(mysql_fd);//关闭mysql文件描述符
    return 0;
}
