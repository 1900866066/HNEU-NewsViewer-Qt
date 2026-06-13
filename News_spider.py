#爬取湖南工程学院新闻网的校园动态

import requests
from lxml import html
import csv


#湖南工程学院新闻网URL
BASE_URL="https://news.hnie.edu.cn/"
#爬取最近多少条信息
n=int(input("请输入要爬取最近多少条新闻："))
#发送get请求
try:
    response=requests.get(BASE_URL,timeout=10)
except:
    print("发送get请求失败")
    exit()
#获取校园动态列表
response.encoding="utf-8"
html1=html.fromstring(response.text)
url=html1.xpath("//div[@class='xydt fl']/ul/li/a/@href")[0]
print(f"获取到首条校园动态url:{url}")
information_list=[]
cur_url=BASE_URL+url
cnt=0
while cnt<n:
    information={}
    try:
        resp=requests.get(cur_url,timeout=10)
    except:
        print("发送get请求失败")
        exit()
    resp.encoding="utf-8"
    cnt+=1
    html2=html.fromstring(resp.text)
    print(f"*请求成功*\n当前url为：{cur_url}")
    print("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
    #标题
    title=html2.xpath("//div[@class='title']/text()")[0]
    print(f"解析到第{cnt}篇新闻标题：{title}")
    #作者
    master=html2.xpath("//form[@name='_newscontent_fromname']/div[2]/i[1]/text()")[0]
    print(f"解析到第{cnt}篇新闻作者：{master}")
    #摄影
    photo=html2.xpath("//form[@name='_newscontent_fromname']/div[2]/i[2]/text()")[0]
    print(f"解析到第{cnt}篇新闻摄影：{photo}")
    #来源
    source=html2.xpath("//form[@name='_newscontent_fromname']/div[2]/i[3]/text()")[0]
    print(f"解析到第{cnt}篇新闻来源：{source}")
    #编辑
    editor=html2.xpath("//form[@name='_newscontent_fromname']/div[2]/i[4]/text()")[0]
    print(f"解析到第{cnt}篇新闻编辑：{editor}")
    #时间
    time=html2.xpath("//form[@name='_newscontent_fromname']/div[2]/text()")[3].split()[0]
    print(f"解析到第{cnt}篇新闻时间：{time}")
    #消息
    news=[]
    new_news=html2.xpath("//*[@id='vsb_content_2']/div/div/p[1]/text()")
    temp=1
    while temp<=15:
        news+=new_news
        temp+=1
        new_news=html2.xpath(f"//*[@id='vsb_content_2']/div/div/p[{temp}]/text()")
    if news==[]:
        temp=1
        new_news=html2.xpath("//*[@class='v_news_content']/p[1]/text()")
        while temp<=15:
            news+=new_news
            temp+=1
            new_news=html2.xpath(f"//*[@class='v_news_content']/p[{temp}]/text()")
    print(f"解析到第{cnt}篇新闻内容：{news}")
    #下一篇url
    next_url=html2.xpath("//div[@class='page_box']/p[2]/a/@href")[0]
    print(f"获取到第{cnt}篇新闻下一篇url：{BASE_URL+next_url}")
    #指向下一篇新闻
    cur_url=BASE_URL+"/info/1004/"+next_url
    print("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<")
    #保存在字典中
    information['title']=title
    information['author']=master
    information['photographer']=photo
    information['source']=source
    information['editor']=editor
    information['time']=time
    information['news']=news
    information_list.append(information)
    print(f"已爬取{cnt}条新闻")
#保存在csv文件中
with open("湖南工程学院校园动态.csv", "w", encoding="utf-8", newline="") as f:
    writer=csv.writer(f)
    writer.writerow(["标题", "作者", "摄影", "来源", "编辑", "时间", "内容"])
    for information in information_list:
        writer.writerow([information['title'], information['author'], information['photographer'], information['source'], information['editor'], information['time'], information['news']])
    print(f"保存成功")