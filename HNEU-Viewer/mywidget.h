#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QThread>
#include <QList>
#include <QMap>
#include <QDialog>
#include <QTextEdit>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
class MyWidget;
}
QT_END_NAMESPACE

class SpiderWorker;

class NewsDetailDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NewsDetailDialog(const QMap<QString, QString> &news, QWidget *parent = nullptr);

private:
    QTextEdit *m_textEdit;
};

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyWidget(QWidget *parent = nullptr);
    ~MyWidget() override;

private slots:
    void onStartSpider();
    void onProgressUpdated(int current, int total, const QString &title);
    void onSpiderFinished(const QList<QMap<QString, QString>> &newsList);
    void onErrorOccurred(const QString &errorMsg);
    void onTableDoubleClicked(int row, int column);

private:
    void initUI();
    void callPythonSpider(int count);
    QList<QMap<QString, QString>> parseCSVFile(const QString &filePath);
    Ui::MyWidget *ui;
    SpiderWorker *m_spiderWorker;
    QThread *m_spiderThread;
    QList<QMap<QString, QString>> m_newsList;
};
#endif // MYWIDGET_H
