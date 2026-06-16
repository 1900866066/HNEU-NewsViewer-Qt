#include "spiderworker.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QRegularExpression>
#include <QTimer>
#include <QDebug>
#include <QTextStream>

SpiderWorker::SpiderWorker(QObject *parent)
    : QObject(parent)
{
}

QString SpiderWorker::fetchPage(const QString &url)
{
    QNetworkAccessManager manager;
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::UserAgentHeader, 
                      QVariant("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36"));
    
    QNetworkReply *reply = manager.get(request);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(10000); // 10秒超时
    
    loop.exec();
    
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg = reply->errorString();
        reply->deleteLater();
        return QString();
    }
    
    QByteArray data = reply->readAll();
    reply->deleteLater();
    return QString::fromUtf8(data);
}

void SpiderWorker::startSpider(int count)
{
    const QString BASE_URL = "https://news.hnie.edu.cn/";
    QList<QMap<QString, QString>> newsList;
    
    qDebug() << "开始爬取，目标数量:" << count;
    
    // 获取首页
    QString homePage = fetchPage(BASE_URL);
    if (homePage.isEmpty()) {
        emit errorOccurred("获取首页失败");
        return;
    }
    
    qDebug() << "首页获取成功，长度:" << homePage.length();
    
    // 提取第一条新闻链接 - 使用更宽松的正则表达式
    QRegularExpression xydtRegex(R"(<div[^>]*class=["'][^"']*xydt[^"']*["'][^>]*>.*?<ul>(.*?)</ul>)", 
                                  QRegularExpression::DotMatchesEverythingOption | QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch xydtMatch = xydtRegex.match(homePage);
    
    QString firstUrl;
    if (xydtMatch.hasMatch()) {
        qDebug() << "找到校园动态区域";
        QString ulContent = xydtMatch.captured(1);
        QRegularExpression linkRegex(R"(<a[^>]*href=["']([^"']+)["'])");
        QRegularExpressionMatch linkMatch = linkRegex.match(ulContent);
        if (linkMatch.hasMatch()) {
            firstUrl = linkMatch.captured(1);
            qDebug() << "找到第一个新闻链接:" << firstUrl;
        }
    } else {
        qDebug() << "未找到校园动态区域，尝试其他匹配方式...";
        // 尝试更宽松的匹配
        QRegularExpression altRegex(R"(<a[^>]*href=["'](/info/1004/[^"']+)["'])");
        QRegularExpressionMatch altMatch = altRegex.match(homePage);
        if (altMatch.hasMatch()) {
            firstUrl = altMatch.captured(1);
            qDebug() << "使用备用方式找到链接:" << firstUrl;
        }
    }
    
    if (firstUrl.isEmpty()) {
        emit errorOccurred("未找到新闻链接，请检查网站结构是否变化");
        return;
    }
    
    QString currentUrl = BASE_URL + firstUrl;
    qDebug() << "开始访问第一条新闻:" << currentUrl;
    
    for (int i = 0; i < count; ++i) {
        qDebug() << "正在爬取第" << (i + 1) << "条新闻...";
        
        QString pageContent = fetchPage(currentUrl);
        if (pageContent.isEmpty()) {
            emit errorOccurred(QString("获取第%1条新闻失败").arg(i + 1));
            break;
        }
        
        QMap<QString, QString> news;
        
        // 提取标题 - 尝试多种匹配方式
        QString title;
        QRegularExpression titleRegex1(R"(<div[^>]*class=["']title["'][^>]*>(.*?)</div>)");
        QRegularExpressionMatch titleMatch1 = titleRegex1.match(pageContent);
        if (titleMatch1.hasMatch()) {
            title = titleMatch1.captured(1);
        } else {
            // 尝试h1标签
            QRegularExpression titleRegex2(R"(<h1[^>]*>(.*?)</h1>)");
            QRegularExpressionMatch titleMatch2 = titleRegex2.match(pageContent);
            if (titleMatch2.hasMatch()) {
                title = titleMatch2.captured(1);
            }
        }
        
        if (!title.isEmpty()) {
            // 去除HTML标签
            title.replace(QRegularExpression("<[^>]*>"), "");
            title = title.trimmed();
            news["title"] = title;
        } else {
            news["title"] = "未知标题";
        }
        
        // 提取发布时间 - 尝试多种格式
        QString time;
        QRegularExpression timeRegex1(R"(发布时间：\s*(\d{4}-\d{2}-\d{2}))");
        QRegularExpressionMatch timeMatch1 = timeRegex1.match(pageContent);
        if (timeMatch1.hasMatch()) {
            time = timeMatch1.captured(1);
        } else {
            // 尝试其他日期格式
            QRegularExpression timeRegex2(R"((\d{4}年\d{1,2}月\d{1,2}日))");
            QRegularExpressionMatch timeMatch2 = timeRegex2.match(pageContent);
            if (timeMatch2.hasMatch()) {
                time = timeMatch2.captured(1);
            } else {
                // 尝试ISO格式
                QRegularExpression timeRegex3(R"((\d{4}-\d{2}-\d{2}))");
                QRegularExpressionMatch timeMatch3 = timeRegex3.match(pageContent);
                if (timeMatch3.hasMatch()) {
                    time = timeMatch3.captured(1);
                }
            }
        }
        
        if (!time.isEmpty()) {
            news["time"] = time;
        } else {
            news["time"] = "未知时间";
        }
        
        // 提取新闻内容 - 参考 Python 代码第58-70行
        QStringList contentList;
        
        // 方式1: 尝试 #vsb_content_2
        QRegularExpression contentRegex1(R"(<div[^>]*id=["']vsb_content_2["'][^>]*>(.*?)</div>)",
                                        QRegularExpression::DotMatchesEverythingOption);
        QRegularExpressionMatch contentMatch1 = contentRegex1.match(pageContent);
        if (contentMatch1.hasMatch()) {
            QString contentDiv = contentMatch1.captured(1);
            // 提取所有p标签的文本
            QRegularExpression pRegex(R"(<p[^>]*>(.*?)</p>)");
            QRegularExpressionMatchIterator it = pRegex.globalMatch(contentDiv);
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                QString pText = match.captured(1);
                pText.replace(QRegularExpression("<[^>]*>"), ""); // 去除HTML标签
                pText = pText.trimmed();
                if (!pText.isEmpty()) {
                    contentList.append(pText);
                }
            }
        }
        
        // 方式2: 如果方式1失败，尝试 .v_news_content
        if (contentList.isEmpty()) {
            QRegularExpression contentRegex2(R"(<div[^>]*class=["']v_news_content["'][^>]*>(.*?)</div>)",
                                            QRegularExpression::DotMatchesEverythingOption);
            QRegularExpressionMatch contentMatch2 = contentRegex2.match(pageContent);
            if (contentMatch2.hasMatch()) {
                QString contentDiv = contentMatch2.captured(1);
                QRegularExpression pRegex(R"(<p[^>]*>(.*?)</p>)");
                QRegularExpressionMatchIterator it = pRegex.globalMatch(contentDiv);
                while (it.hasNext()) {
                    QRegularExpressionMatch match = it.next();
                    QString pText = match.captured(1);
                    pText.replace(QRegularExpression("<[^>]*>"), "");
                    pText = pText.trimmed();
                    if (!pText.isEmpty()) {
                        contentList.append(pText);
                    }
                }
            }
        }
        
        // 将内容列表合并为字符串
        QString fullContent = contentList.join("\n\n");
        if (fullContent.isEmpty()) {
            fullContent = "暂无详细内容";
        }
        news["content"] = fullContent;
        
        qDebug() << "第" << (i + 1) << "条新闻:" << news["title"] << "|" << news["time"] << "| 内容长度:" << fullContent.length();
        
        newsList.append(news);
        
        qDebug() << "第" << (i + 1) << "条新闻:" << news["title"] << "|" << news["time"];
        
        emit progressUpdated(i + 1, count, news["title"]);
        
        // 提取下一篇新闻链接 - 参考 Python 爬虫的实现
        QString nextPath;
        
        // 方式1: 匹配page_box中的p[2]/a（参考Python代码第73行）
        QRegularExpression nextUrlRegex1(R"(<div[^>]*class=["']page_box["'][^>]*>.*?<p[^>]*>[^<]*<a[^>]*href=["']([^"']+)["'])",
                                        QRegularExpression::DotMatchesEverythingOption);
        QRegularExpressionMatch nextUrlMatch1 = nextUrlRegex1.match(pageContent);
        
        if (nextUrlMatch1.hasMatch()) {
            nextPath = nextUrlMatch1.captured(1);
            qDebug() << "使用方式1找到下一页:" << nextPath;
        } else {
            // 方式2: 直接匹配包含"下一篇"文字的链接
            QRegularExpression nextUrlRegex2(R"(<a[^>]*href=["']([^"]+)["'][^>]*>[^<]*下一篇)");
            QRegularExpressionMatch nextUrlMatch2 = nextUrlRegex2.match(pageContent);
            if (nextUrlMatch2.hasMatch()) {
                nextPath = nextUrlMatch2.captured(1);
                qDebug() << "使用方式2找到下一页:" << nextPath;
            } else {
                // 方式3: 匹配所有.htm链接，取最后一个
                QRegularExpression nextUrlRegex3(R"(<a[^>]*href=["']([^"]+\.htm)["'])");
                QRegularExpressionMatchIterator it = nextUrlRegex3.globalMatch(pageContent);
                QString lastMatch;
                while (it.hasNext()) {
                    QRegularExpressionMatch match = it.next();
                    lastMatch = match.captured(1);
                }
                if (!lastMatch.isEmpty()) {
                    nextPath = lastMatch;
                    qDebug() << "使用方式3找到下一页:" << nextPath;
                }
            }
        }
        
        if (nextPath.isEmpty()) {
            qDebug() << "没有更多新闻了，停止爬取";
            break; // 没有更多新闻了
        }
        
        // 处理相对路径 - 参考 Python 代码第76行: cur_url=BASE_URL+"/info/1004/"+next_url
        if (nextPath.startsWith("../")) {
            nextPath = nextPath.mid(3);
        }
        // 如果已经是完整路径或包含info/1004，直接使用
        if (nextPath.contains("info/1004/")) {
            currentUrl = BASE_URL + nextPath;
        } else {
            // 否则添加 /info/1004/ 前缀
            currentUrl = BASE_URL + "/info/1004/" + nextPath;
        }
        
        qDebug() << "下一条URL:" << currentUrl;
    }
    
    qDebug() << "爬取完成，共获取" << newsList.size() << "条新闻";
    emit spiderFinished(newsList);
}
