#!/usr/bin/env python
# coding=utf-8

import urllib2
from bs4 import BeautifulSoup
import re

import sys
reload(sys)
sys.setdefaultencoding('utf8')

# 1.或取主页面内容也可以用于获取详情页
def OpenPage(url):
    '''
    根据url构造HTTP请求发送给服务器
    或取到HTTP服务器的响应
    '''
    #为了构造请求对象所构造的header对象
    #headers = {}
    #构造请求对象
    req = urllib2.Request(url)
    #发送http请求,获取到一个文件对象
    f = urllib2.urlopen(req)
    #从文件中读取返回结果
    data = f.read()
    #decode操作是把GBK编码格式转成unicode的编码格式
    #encode操作是把unicode转成utf-8格式
    data = data.decode('GBK',errors='ignore').encode('UTF-8')
    return data

def Test1():
    url = 'http://www.shengxu6.com/book/2967.html'
    print OpenPage(url)

def ParseMainPage(page):
    soup = BeautifulSoup(page,'html.parser')
    #筛选出所有符合条件的a标签规则：
    #查找所有带有href属性的标签，并且href中包含的url又包含"read"关键字
    chapter_list = soup.find_all(href=re.compile('read'))
    #获取出a标签中的url
    url_list = ['http://www.shengxu6.com' + item['href']for item in chapter_list]
    return url_list 

def Test2():
    url = 'http://www.shengxu6.com/book/2967.html'
    html = OpenPage(url)
    ParseMainPage(html)

def Test3():
    url = 'http://www.shengxu6.com/read/2967_2008684.html'
    html = OpenPage(url)
    print html

def ParseDetailPage(page):
    soup = BeautifulSoup(page,'html.parser')
    result = soup.find_all(class_='content-body')[0].get_text()
    return result[:-len('_drgd200();')-1]


def Test4():
    url = 'http://www.shengxu6.com/read/2967_2008684.html'
    html = OpenPage(url)
    result = ParseDetailPage(html)
    print result

def Write(file_path,data):
    with open(file_path,'a+') as f:
        f.write(data)

def Run():
    '''
    整个函数的入口函数
    '''
    #1.获取到整个小说的主页面
    main_url = 'http://www.shengxu6.com/book/2967.html'
    page = OpenPage(main_url)
    #2.根据主页面解析出所有详细页的url
    url_list = ParseMainPage(page)
    #3.遍历详细页，获取到每个详细页的小说内容
    for url in url_list:
        print 'for循环'
        detail_page = OpenPage(url)
        result = ParseDetailPage(detail_page)
    #4.把详细页的内容写到文件中
        Write('./result.txt',result)
        print '写到文件'


if __name__ == '__main__':
    Run()
