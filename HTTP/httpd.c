#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<strings.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<sys/sendfile.h>
#include<ctype.h>
#define MAX 1024
#define HOME_PAGE "index.html"
//首先设置结构体关键字
//创建套接字
//绑定套接字
//监听套接字
//接收套接字
//创建多线程处理套接字(分析接收到的套接字的http协议中的方法)
static void usage(const char* proc)//用static修饰，只在本函数内部有效//用static修饰，只在本函数内部有效
{
    printf("Usage:%s port\n",proc);//告诉客户端应该如何使用
}
static int startup(int port)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0)//创建失败
    {
        //服务器是以后台进程运行的，正常情况下这里不应该perror
        //而应该把错误信息打印到日志当中
        perror("socket");
        //创建套接字失败的话就不再继续向下运行
        exit(2);
    }
    //保证服务器断开连接的时候，不能让服务器因为time_wait而不能立即重启
    //所以要调用setsockopt，对套接字属性进行设置
    int opt = 1;
    setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    //第一个参数要设置的套接字
    //第二个参数要设置的层数
    //第三个是要干什么，现在的情况是需要端口可以复用
    struct sockaddr_in local;//填充本地消息
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);//绑定本地任意ip
    local.sin_port = htons(port);
    if(bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0 )//绑定失败
    {
        perror("bind");
        exit(3);
    }
    if(listen(sock,5) < 0)//监听
    {
        perror("listen");

        exit(4);
    }
    return sock;
}
//客户端可能发来的行分隔符有三种  \r  \n  \r\n
//统一按行'\n'来处理,一次读取一个字符
//按行获得文本，1.要从哪个套接字获得 2.获得之后要把获得的写到哪个缓冲区里 3.缓冲区多大
int get_line(int sock,char line[],int size)
    //从哪个套接字获得，将获得到的消息写到哪个缓冲区里，缓冲区多大
{
    int c = 'a';//只要初始化的值不是'\n'就可以
    int i = 0;
    ssize_t s = 0;
    while( i < size-1 && c != '\n')//因为最后一个应该放'\0'所以i要小于size-1
    {
        s = recv(sock,&c,1,0);//一次读1个，目的:将分隔符进行转化 0：代表以阻塞方式读
        if(s > 0)
        {
            if(c == '\r')//如果c是'\r'
            {
                // \r -> \n  or  \r\n -> \n
                recv(sock,&c,1,MSG_PEEK);//用MSG_PEEK进行窥探下一个元素
                if(c != '\n')//如果下一个字符不是'\n'说明此时的行分隔符只有'\r'
                {
                    c = '\n';//读进来的是'\r'，将'\r'转成'\n'
                }
                else//否则说明下一个字符是'\n'，但是'\n'还在缓冲区里，这次读不会被阻塞
                {
                    recv(sock,&c,1,0);
                }
            }
            //c == \n
            line[i++] = c;// \n 
        }
        else
        {
            //读取失败
            break;
        }
    }
    line[i] = '\0';
    return i;//返回这一行有多少个字符
    //如果是空行只有'\n'
    //原因：如果是'\r'窥探下一个字符，是'\n'就再读取所以c变为'\n'
    //如果窥探的下一个字符不是'\n'就用'\n'替换'\r'
    //所以无论如何空行读到的都是'\n'
}

void clear_header(int sock)
{
    char line[MAX] = {0};
    do{
        get_line(sock,line,sizeof(line));
        //printf("%s",line);
    }while(strcmp(line,"\n") != 0);
}
void echo_www(int sock,char* path,int size,int *err)
{
    clear_header(sock);//读到空行停下来,请求处理完了

    int fd = open(path,O_RDONLY);
    if(fd < 0)
    {
        *err = 404;
        return;
    }
    char line[MAX];
    printf("fd:%d size:%d\n",fd,size);
    sprintf(line,"HTTP/1.0 200 OK\r\n");
    send(sock,line,strlen(line),0);
    sprintf(line,"Content_Type: text/html\r\n");
    send(sock,line,strlen(line),0);
    sprintf(line,"\r\n");
    send(sock,line,strlen(line),0);
    sendfile(sock,fd,NULL,size);
    close(fd);
}
void bad_request(const char* path,const char* head,int sock)
{
    clear_header(sock);
    struct stat st;
    if(stat(path,&st) < 0)
    {
        return;
    }
    int fd = open(path,O_RDONLY);
    if(fd < 0)
    {
        perror("open\n");
        return;
    }
    printf("path = %s\n",path);
    //响应报头正文
    const char* status_line = head;
    send(sock,status_line,strlen(status_line),0);
    printf("%s\n",status_line);
    const char* content_type = "Content-Type:text/html;charset=ISO-8859-1\r\n";
    send(sock,content_type,strlen(content_type),0);
    send(sock,"\r\n",2,0);
    sendfile(sock,fd,NULL,st.st_size);
    close(fd);
}
void echo_error(int sock,int error_code)
{
    switch(error_code)
    {
        case 400:
            bad_request("wwwroot/400.html","HTTP/1.0 404 Not Found\r\n",sock);
            break;
        case 403:
            bad_request("wwwroot/403.html","HTTP/1.0 404 Not Found\r\n",sock);
            break;
        case 404:
            bad_request("wwwroot/404.html","HTTP/1.0 404 Not Found\r\n",sock);
            break;
        case 500:
            bad_request("wwwroot/500.html","HTTP/1.0 404 Not Found\r\n",sock);
            break;
        case 503:
            bad_request("wwwroot/503.html","HTTP/1.0 404 Not Found\r\n",sock);
            break;
        default:
            break;
    }
}
//网页显示
void echo_info(int sock,char* path,int size,int* error_code)
{
    clear_header(sock);
    //响应
    //sendfile高效，因为read需要先把数据读到内核，然后从内核把数据读到用户
    //但是sendfile不用在用户间来回操作，只在两个文件描述符之间来回操作，所以更加高效
    //1.打开文件
   int fd = open(path,O_RDONLY);
   if(fd < 0)
   {
       //文件打开失败
       *error_code = 404;
       return;
   }
   char line[MAX] = {0};
   sprintf(line,"HTTP/1.0 200 OK\r\n");
   //将状态行发出去
   send(sock,line,strlen(line),0);
   sprintf(line,"Content_Type:text/html;charset=ISO-8859-1\r\n");
   send(sock,line,strlen(line),0);
   sprintf(line,"\r\n");
   send(sock,line,strlen(line),0);
   sendfile(sock,fd,NULL,size);
   close(fd);
}

int exe_cgi(int sock,char* path,char* method,char* query_string)
{
    //判断是哪个方法
    char line[MAX] = {0};
    int content_length = -1;
    char method_env[MAX/32];
    char query_string_env[MAX];
    char content_length_env[MAX/16];
    if(strcasecmp(method,"GET") == 0)
    {
        clear_header(sock);
        //参数在query_string中
    }
    if(strcasecmp(method,"POST") == 0)
    {
        //POST
        do
        {
            get_line(sock,line,sizeof(line));
            if(strncmp(line,"Content-Length: ",16) == 0)
            {
                content_length = atoi(line + 16);
            }
        }while(strcmp(line,"\n") != 0);
        //再读就读到正文了
        if(content_length == -1)
        {
            //说明没有读到Content-Length,出错了
            return 400;
        }
    }
    sprintf(line,"HTTP/1.0 200 OK\r\n");
    //将状态行发出去
    send(sock,line,strlen(line),0);
    sprintf(line,"Content-Type: text/html;charset=ISO-8859-1\r\n");
    send(sock,line,strlen(line),0);
    sprintf(line,"\r\n");
    send(sock,line,strlen(line),0);
    //父进程要拿到子进程的运行结果，此时就需要进程间通信
    //让父进程将数据拿到写给子进程，子进程将运行结果写给父进程
    int input[2];//子进程用input来读，父进程用input来写
    int output[2];//子进程用output来写，父进程用output来读

    pipe(input);
    pipe(output);

    pid_t id = fork();
    if(id < 0)
    {
        return 503;
    }
    else if(id == 0)
    {
        //程序替换不会替换环境变量
        //子进程替换
        //子进程要从管道中读
        close(input[1]);
        close(output[0]);
        //重定向
        //程序替换时不会替换文件描述符
        //文件描述符是属于一个进程打开的一个概念，不是代码和数据,所以不会替换
        dup2(input[0],0);
        dup2(output[1],1);
    
        //导出环境变量
        //环境变量具有全局特性，会被子进程继承
        //环境变量也是子进程的数据，在程序替换时不会被替换掉
        sprintf(method_env,"METHOD=%s",method);
        putenv(method_env);//把指定的环境变量导到子进程的地址空间上
    
        if(strcasecmp(method,"GET") == 0)
        {
            sprintf(query_string_env,"QUERY_STRING=%s",query_string);
            putenv(query_string_env);
        }
        else
        {
            sprintf(content_length_env,"CONTENT_LENGTH=%d",content_length);
            putenv(content_length_env);
        }
        execl(path,path,NULL);//第一个path，要执行谁，第二个往后的参数，要如何执行
        //什么权限都不用传所以为NULL
        exit(1);
    }
    else
    {
        //父进程
        close(input[0]);
        close(output[1]);
        int c;
        if(strcasecmp(method,"POST") == 0)
        {
            int i = 0;
            while(i < content_length)
            {
                read(sock,&c,1);
                write(input[1],&c,1);
                i++;
            }
        }
        while(read(output[0],&c,1) > 0)
        {
            send(sock,&c,1,0);
        }
        //从sock中读数据
        waitpid(id,NULL,0);
        //父进程关闭自己的文件描述符
        close(input[1]);
        close(output[0]);
    }
    return 200;
    
}
static void* handler_request(void* arg)
{
    int sock1 = (int)arg;
    int sock = sock1;
    char line[MAX] = {0};
    char method[MAX/32];//方法
    char url[MAX];//请求的资源
    char path[MAX];//资源路径
    int errCode = 200;//状态码
    int cgi = 0;//cgi 通用网关接口，是Http服务内置的一种标准，方便后对Http的功能进行二次扩展
    char* query_string = NULL;
#if Debug
    do{
        get_line(sock,line,sizeof(line));//line -> \r,\n,\r\n -->\n
        printf("%s",line);
    }while(strcmp(line,"\n") != 0);
#else
    //在向上交数据时，如果数据被显示到url中时，这种方法叫做GET方法
    //在向上交数据时，数据不会被显示在url中,而放在了正文部分,这种方法叫做POST方法
    //首先要把第一行拿到
    //GET方法可以有参数可以没参数，而POST方法在Http中必须必须得有cgi方式运行
    if(get_line(sock,line,sizeof(line)) < 0)//获得行出错
    {
        errCode = 404;//错误码
        goto end;//第一行读取都有错误，http不能处理，直接不处理
    }
    //第一行获取成功
    //第一个提取方法字段
    int i = 0;
    int j = 0;
    while(i < (int)sizeof(method) - 1 && j < (int)sizeof(line) && !isspace(line[j]))
    {
        method[i] = line[j];
        i++;
        j++;
    }
    method[i] = '\0';
    if(strcasecmp(method,"GET") == 0)//忽略大小写比较
    {
        
    }
    else if(strcasecmp(method,"POST") == 0)//一旦是POST方法就要以cgi方式运行
    {
        cgi = 1;
    }
    else//出错
    {
        errCode = 400;
        goto end;
    }
    //走到这里要么是GET方法，要么是POST方法
    //提取url
    while(j < (int)sizeof(line) && isspace(line[j]))//此时的j是空格
    {
        j++;
    }
    //j指向一个非空字符
    
    
    //上一次i循环指向method，现在想让i指向url，所以需要把i清0
    i = 0;
    while(i < (int)sizeof(url)-1 && j < (int)sizeof(line) && !isspace(line[j]))
    {
        url[i] = line[j];
        i++;
        j++;
    }
    url[i] = '\0';
    //方法要么是GET要么是POST，而且资源已经拿到
    //GET方法和POST方法在传参形式上不同
    //GET方法通过url传参，POST方法通过请求正文传参
    //'?'后面传的是参数，参数内部是 name=value,而多个参数之间用&隔开
    //检测当前是否带问号(GET方法才进行判断)
    //从应用上一种是将资源从网络上拉下来，另一种是将自己的数据提交到网络上，这就叫cgi
    //GET：当向网络上提交数据的时候将数据拼接到URL后
    //POST：将参数放到http的正文部分
    //如果url中没有参数，正文中也没有参数，则是GET
    //如果url中带参数了，是GET方法
    //如果正文中带参就是POST
    //如果带参数了，就必须对这个参数进行处理,就需要以cgi方式运行
    //如果是POST，就必须以cgi方式运行
    
    //判断方法：要么是GET，要么是POST
    //在http请求中如果要用get方法进行传参，那么就会给其在url中加上'?'
    //'?'左侧是访问的资源，'?'右侧是给资源的参数
    //整体都在url中
    //其中请求的资源可能是一个可执行程序
    //
    //检测当前url是否有'?'
    //将'?'后面的叫做query_string
    //将url指向'?'前半部分
    if(strcasecmp(method,"GET") == 0)
    {
        query_string = url;
        while(*query_string)
        {
            if(*query_string == '?')
            {
                *query_string = '\0';
                query_string++;
                //GET方法带参用cgi
                cgi = 1;
                break;
            }
            query_string++;
        }
    }
    //method[GET,POST], cgi[0|1],url[],query_string[NULL|arg]
    //url -> wwwroot/a/b/c.html | url -> wwwroot/
    //判断请求资源存在还是不存在
    //请求资源从wwwroot发出
    //url->wwwroot/a/b/c.html或者url->wwwroot
    //拼接路径
    sprintf(path,"wwwroot%s",url);
    if(path[strlen(path)-1] == '/')//如果最后一个字符是'/'就把首页给拼上
    {
        strcat(path,HOME_PAGE);
    }
    //判断要请求的资源是否存在
    printf("method = %s,path: %s\n",method,path);
    struct stat st;
    if(stat(path,&st) < 0)//要请求的资源
    {
        //说明访问的文件不存在
        errCode = 404;
        goto end;
    }
    else//文件找到了,如果文件具有可执行权限，就要以cgi的方式运行
        //三个组只要有一个有可执行权限，就认为有可执行权限
    {
        if(S_ISDIR(st.st_mode))//如果是目录
        {
            strcat(path,HOME_PAGE);//如果是目录就把首页拼上
        }
        //文件找到了
        //但是找到不一定是正确的
        //如果访问资源是一个可执行的，就以cgi方式运行
        //判断文件是否具有可执行权限
        else
        {
            if((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
            {
                cgi = 1;
            }
        }
            //能判断出三点
            //第一资源肯定存在
            //第二资源肯定不是目录
            //第三资源肯定没有可执行程序
        if(cgi)
        {
            exe_cgi(sock,path,method,query_string);
        }
        else//GET方法并且没有传参
        {
            echo_www(sock,path,st.st_size,&errCode);
        }

    }
    
#endif
end:
    if(errCode != 200)
    {
        echo_error(sock,errCode);//往哪个链接中传，传哪个错误码
    }
    close(sock);//作用：1.回收了本地描述符资源   2.关闭了连接
}

int main(int argc,char* argv[])
{
    //./httpd 80
    if(argc != 2)
    {
        usage(argv[0]);
        return 1;
    }
    //现在的Http服务器底层是基于TCP的，所以第一件事一定要有一个listen_sock
    int listen_sock = startup(atoi(argv[1]));//调用start来获得监听套接字
    //监听套接字有了就进行事件处理
    for(;;)
    {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        int new_sock = accept(listen_sock,(struct sockaddr*)&client,&len);//获取连接
        //从listen_sock中获取新连接，获得新连接放到client中
        if(new_sock < 0)
        {
            perror("accept");
            continue;//继续再获取新连接
        }
        //获取连接成功
        pthread_t id;
        //printf("获取连接成功！\n");
        pthread_create(&id,NULL,handler_request,(void*)new_sock);//创建线程
        //线程属性NULL，创建这个线程为了完成处理请求及其其响应的,提供服务
        //(我们的浏览器在向我发起http请求之前要先建立连接，一旦建立连接
        //那么服务器就获得连接并创建新线程，新线程剩下的工作就是处理请求)
        //new_sock : 处理的是哪一个连接(直接传值的时候可能会遇到一些问题
        //强转成void* 以只拷贝的形式传过去)
        pthread_detach(id);//分离，继续获取新连接
    }
    return 0;
}
