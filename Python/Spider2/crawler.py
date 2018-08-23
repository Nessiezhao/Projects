#!/usr/bin/env python
# coding=utf-8
from bs4 import BeautifulSoup
import urllib2
import json
import MySQLdb
import base64

def OpenPage(url):
    req = urllib2.Request(url)
    f = urllib2.urlopen(req)
    return f.read()


def Test1():
    url = 'http://jy.51uns.com:8022/Pro_StudentEmploy/StudentJobFair/Zhaoping.aspx?WorkType'
    html = OpenPage(url)
    print html


def Test2():
    url = 'http://jy.51uns.com:8022/Frame/Data/jdp.ashx?rnd=1535005192986&fn=GetZhaopinList&StartDate=2000-01-01&SearchKey=&InfoType=-1&CompanyAttr=&CompanyType=&Area=&City=&CompanyProvice=&Post=&Zhuanye=&XLkey=&Age=&start=0&limit=15&DateType=999&InfoState=1&WorkType=0&CompanyKey='
    html = OpenPage(url)
    print html

def ParseMainPage(page):
    '''
    本函数的目标是为了或取到所有招聘信息的id
    得到了id后，才能拼接出详细页的url
    解析得到的json结构，遍历其中的结果集合把id解析出来
    就可以构造url
    '''
    #json.loads将page这个字符串转换成了python中的字典结构
    data = json.loads(page)
    rows = data['rows']
    url_prefix = 'http://jy.51uns.com:8022/Pro_StudentEmploy/StudentJobFair/Zhaoping_Detail.aspx?JobId='
    return [url_prefix + row['Id'] for row in rows]

def Test3():
    url = 'http://jy.51uns.com:8022/Frame/Data/jdp.ashx?rnd=1535005192986&fn=GetZhaopinList&StartDate=2000-01-01&SearchKey=&InfoType=-1&CompanyAttr=&CompanyType=&Area=&City=&CompanyProvice=&Post=&Zhuanye=&XLkey=&Age=&start=0&limit=15&DateType=999&InfoState=1&WorkType=0&CompanyKey='
    html = OpenPage(url)
    print ParseMainPage(html)


def Test4():
    '''
    验证招聘信息的详情也是否能正确运行
    '''
    url = 'http://jy.51uns.com:8022/Frame/Data/jdp.ashx?rnd=1535006490883&fn=GetOneZhaopin&StartDate=2000-01-01&JobId=6c486989f5424026a214c074cf038f3d'
    print OpenPage(url)

def ParseDetailPage(page):
    '''
    此时最关注的字段有：
        1.公司名称：CompanyTitle
        2.薪酬待遇：WorkPrice
        3.工作岗位：WorkPosition
        4.工作职责：EmployContent
    '''
    data = json.loads(page)
    data = data['Data']
    content = data['EmployContent']
    soup = BeautifulSoup(content,'html.parser') 
    result = soup.find_all('p')
    text = '\n'.join([item.get_text() for item in result])
    return (data['CompanyTitle'], data['WorkPrice'], 
            data['WorkPosition'], content)


def Test5():
    url = 'http://jy.51uns.com:8022/Frame/Data/jdp.ashx?rnd=1535006490883&fn=GetOneZhaopin&StartDate=2000-01-01&JobId=6c486989f5424026a214c074cf038f3d'
    page = OpenPage(url)
    result = ParseDetailPage(page)
    print result[0], result[1], result[2], result[3]


def WriteData(data):
    '''
    把data数据写到数据库中
    data是一个四元组，四元组中包含公司名称，薪酬待遇，工作岗位，工作职责
    ORM基于面向对象的思想来操作数据库，创建一个对象和数据库的某个表关联到一起，修改对象中字段的值
    '''
    #1.先和数据库建立连接
    db = MySQLdb.connect(host='127.0.0.1',user='root',passwd='root',db='TestPy',charset='utf8')
    #2.构造cursor对象
    cursor = db.cursor()
    #3.构造sql语句
    #由于sql中如果包含了特殊字符，例如单引号，可能会导致出错，所以需要对字符串进行base64编码
    content = base64.b64encode(data[3])
    sql = "insert into CrawlerSchool values('%s','%s','%s','%s')" % (
            data[0], data[1], data[2], content)
    print 'sql=', sql
    #4.执行sql语句
    cursor.execute(sql)
    #5.把请求提交到数据库服务器
    db.commit()
    #6.关闭数据库连接
    db.close()


def Test6():
    data = ('比特科技','6k~8k','班主任小姐姐','人美声音甜')
    WriteData(data)

def Run():
    '''
    整个程序的入口
    '''
    #1.或取到主业内容
    main_url = 'http://jy.51uns.com:8022/Pro_StudentEmploy/StudentJobFair/Zhaoping.aspx?WorkType'
    page = OpenPage(main_url)
    # 根据主页内容，解析出所有详情页的url
    url_list = ParseMainPage(page)
    # 遍历所有详情页的url
    for url in url_list:
        detail_page = OpenPage(url)
        result = ParseDetailPage(detail_page)
    # 分别或取到每个详情页的内容，并解析
    # 把解析结果写到数据库
    WriteData('./result.txt',result)
    # 详情页需要通过这个url来或取到
if __name__ == '__main__':
    Run()
