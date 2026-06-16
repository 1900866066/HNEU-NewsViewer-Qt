#ifndef SPIDERWORKER_H
#define SPIDERWORKER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>

class SpiderWorker : public QObject
{
    Q_OBJECT
public:
    explicit SpiderWorker(QObject *parent = nullptr);

signals:
    void progressUpdated(int current, int total, const QString &title);
    void spiderFinished(const QList<QMap<QString, QString>> &newsList);
    void errorOccurred(const QString &errorMsg);

public slots:
    void startSpider(int count);

private:
    QString fetchPage(const QString &url);
};

#endif // SPIDERWORKER_H
