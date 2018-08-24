#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<mysql.h>
#include <dlfcn.h>
#include <ctemplate/template.h>
void test()
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
    //printf("connect mysql success!\n");
    //下发sql命令
    char sql[1024];
    sprintf(sql,"select * from student_info");
    mysql_query(mysql_fd,sql);
    //读取结果
    MYSQL_RES* res = mysql_store_result(mysql_fd);
    //获取当前表对应的行
    int row = mysql_num_rows(res);
    //获取当前表对应的列
    int col = mysql_num_fields(res);
    //获取当前表对应的列名
    MYSQL_FIELD* field = mysql_fetch_fields(res);
    int i = 0;
    ctemplate::TemplateDictionary dict("/home/xiaobuding/Git/Projects/HTTP/wwwroot/test.tpl");
//    dict.SetValue("TABLE", "/home/xiaobuding/Git/Projects/HTTP/wwwroot/test.tpl");
    dict.SetValue("KEY", "VALUE");
    for(i = 1;i <= row;i++)
    {
        ctemplate::TemplateDictionary* table1_dict;
        table1_dict = dict.AddSectionDictionary("TABLE");
        //获取结果内容
        MYSQL_ROW rowData = mysql_fetch_row(res);
        table1_dict->SetFormattedValue("field0", "%d", i);
        table1_dict->SetValue("field1",rowData[0]);
        table1_dict->SetValue("field2",rowData[1]);
        table1_dict->SetValue("field3",rowData[2]);
    }
    std::string output;
    ctemplate::Template* tpl;
    tpl = ctemplate::Template::GetTemplate("/home/xiaobuding/Git/Projects/HTTP/wwwroot/test.tpl", ctemplate::DO_NOT_STRIP);
    tpl->Expand(&output, &dict);
    printf("%s\n", output.c_str());
    mysql_close(mysql_fd);//关闭mysql文件描述符
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
