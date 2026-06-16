#include "mywidget.h"
#include "ui_mywidget.h"
#include "spiderworker.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QDebug>
#include <QFileInfo>
#include <QDirIterator>
#include <QProcess>
#include <QTableWidgetItem>
#include <QPixmap>

// NewsDetailDialog 实现
NewsDetailDialog::NewsDetailDialog(const QMap<QString, QString> &news, QWidget *parent)
    : QDialog(parent)
    , m_textEdit(new QTextEdit(this))
{
    setWindowTitle("新闻详情");
    resize(800, 600);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(15, 15, 15, 15);
    
    // 标题
    QLabel *titleLabel = new QLabel(news.value("title", "无标题"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setWordWrap(true);
    titleLabel->setStyleSheet("color: #333;");
    layout->addWidget(titleLabel);
    
    // 时间
    QLabel *timeLabel = new QLabel("发布时间：" + news.value("time", "未知"), this);
    QFont timeFont = timeLabel->font();
    timeFont.setPointSize(10);
    timeFont.setItalic(true);
    timeLabel->setFont(timeFont);
    timeLabel->setStyleSheet("color: #666;");
    layout->addWidget(timeLabel);
    
    // 分隔线
    QLabel *line = new QLabel(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #ddd;");
    layout->addWidget(line);
    
    // 元信息区域（作者、摄影、来源、编辑）
    QFrame *metaFrame = new QFrame(this);
    metaFrame->setStyleSheet("QFrame { background-color: #f9f9f9; border-radius: 5px; padding: 10px; }");
    QVBoxLayout *metaLayout = new QVBoxLayout(metaFrame);
    metaLayout->setSpacing(5);
    
    if (!news.value("author").isEmpty() && news.value("author") != "\"") {
        QLabel *authorLabel = new QLabel("作者：" + news.value("author"), metaFrame);
        authorLabel->setStyleSheet("color: #555; font-size: 10pt;");
        metaLayout->addWidget(authorLabel);
    }
    
    if (!news.value("photographer").isEmpty() && news.value("photographer") != "\"") {
        QLabel *photoLabel = new QLabel("摄影：" + news.value("photographer"), metaFrame);
        photoLabel->setStyleSheet("color: #555; font-size: 10pt;");
        metaLayout->addWidget(photoLabel);
    }
    
    if (!news.value("source").isEmpty() && news.value("source") != "\"") {
        QLabel *sourceLabel = new QLabel("来源：" + news.value("source"), metaFrame);
        sourceLabel->setStyleSheet("color: #555; font-size: 10pt;");
        metaLayout->addWidget(sourceLabel);
    }
    
    if (!news.value("editor").isEmpty() && news.value("editor") != "\"") {
        QLabel *editorLabel = new QLabel("编辑：" + news.value("editor"), metaFrame);
        editorLabel->setStyleSheet("color: #555; font-size: 10pt;");
        metaLayout->addWidget(editorLabel);
    }
    
    layout->addWidget(metaFrame);
    
    // 内容区域
    m_textEdit->setReadOnly(true);
    m_textEdit->setFont(QFont("Microsoft YaHei", 11));
    m_textEdit->setStyleSheet("QTextEdit { border: none; background-color: #fafafa; padding: 10px; color: #333; }");
    
    // 解析并显示新闻内容（CSV中的内容是Python列表格式字符串）
    QString content = news.value("content", "暂无详细内容");
    
    // 移除首尾的方括号和引号
    content = content.trimmed();
    if (content.startsWith("['") && content.endsWith("']")) {
        content = content.mid(2, content.length() - 4);
    } else if (content.startsWith("[") && content.endsWith("]")) {
        content = content.mid(1, content.length() - 2);
    }
    
    // 替换转义字符
    content.replace("\\'", "'");
    content.replace("\\n", "\n");
    
    // 分割段落（用逗号分隔的字符串）
    QStringList paragraphs = content.split("', '");
    QString formattedContent;
    for (const QString &para : paragraphs) {
        QString p = para.trimmed();
        if (!p.isEmpty()) {
            formattedContent += p + "\n\n";
        }
    }
    
    if (formattedContent.isEmpty()) {
        formattedContent = content;
    }
    
    m_textEdit->setPlainText(formattedContent);
    
    layout->addWidget(m_textEdit);
    
    setLayout(layout);
}

// MyWidget 实现

MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyWidget)
    , m_spiderWorker(nullptr)
    , m_spiderThread(nullptr)
{
    ui->setupUi(this);
    initUI();
    
    // 创建爬虫工作对象和线程
    m_spiderThread = new QThread(this);
    m_spiderWorker = new SpiderWorker();
    m_spiderWorker->moveToThread(m_spiderThread);
    m_spiderThread->start();
    
    // 连接信号槽
    connect(ui->pushButton, &QPushButton::clicked, this, &MyWidget::onStartSpider);
    connect(m_spiderWorker, &SpiderWorker::progressUpdated, this, &MyWidget::onProgressUpdated);
    connect(m_spiderWorker, &SpiderWorker::spiderFinished, this, &MyWidget::onSpiderFinished);
    connect(m_spiderWorker, &SpiderWorker::errorOccurred, this, &MyWidget::onErrorOccurred);
    
    // 连接表格双击信号
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &MyWidget::onTableDoubleClicked);
}

MyWidget::~MyWidget()
{
    if (m_spiderThread && m_spiderThread->isRunning()) {
        m_spiderThread->quit();
        m_spiderThread->wait();
    }
    delete ui;
}

void MyWidget::initUI()
{
    setWindowTitle("湖南工程学院新闻网爬虫");
    resize(1200, 800);
    
    // 设置整体样式
    setStyleSheet(R"(
        QWidget {
            background-color: #f0f2f5;
            font-family: "Microsoft YaHei", "SimSun", sans-serif;
        }
        QFrame {
            background-color: white;
            border-radius: 10px;
            padding: 15px;
            border: 1px solid #e0e0e0;
        }
        QLabel {
            color: #333;
        }
        QLineEdit {
            padding: 8px 12px;
            border: 2px solid #d9d9d9;
            border-radius: 6px;
            font-size: 13px;
            background-color: #fafafa;
            color: #333;
        }
        QLineEdit:focus {
            border-color: #4CAF50;
            background-color: white;
            color: #333;
        }
        QPushButton {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                              stop:0 #5cb85c, stop:1 #4cae4c);
            color: white;
            border: none;
            border-radius: 6px;
            padding: 10px 20px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                              stop:0 #4cae4c, stop:1 #45a049);
        }
        QPushButton:pressed {
            background-color: #3d8b40;
        }
        QPushButton:disabled {
            background-color: #cccccc;
            color: #666666;
        }
        QTableWidget {
            border: 2px solid #ddd;
            border-radius: 6px;
            gridline-color: #e8e8e8;
            background-color: white;
            color: #333;
            selection-background-color: #e3f2fd;
        }
        QTableWidget::item {
            padding: 8px;
            border-bottom: 1px solid #f0f0f0;
            color: #333;
        }
        QTableWidget::item:selected {
            background-color: #e3f2fd;
            color: #333;
        }
        QHeaderView::section {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                              stop:0 #4CAF50, stop:1 #45a049);
            color: white;
            padding: 10px;
            border: none;
            border-right: 1px solid rgba(255,255,255,0.3);
            font-weight: bold;
            font-size: 12px;
        }
        QHeaderView::section:hover {
            background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                              stop:0 #4cae4c, stop:1 #45a049);
        }
        QScrollBar:vertical {
            border: none;
            background: #f5f5f5;
            width: 12px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: #c1c1c1;
            min-height: 20px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical:hover {
            background: #a8a8a8;
        }
    )");
    
    // 设置表格样式
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableWidget->setColumnWidth(0, 80);
    ui->tableWidget->setColumnWidth(2, 260);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(false);
    ui->tableWidget->setWordWrap(true);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setShowGrid(true);

    // 显式设置表头（确保标题顺序：序号, 标题, 发布时间）
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << QStringLiteral("序号") << QStringLiteral("新闻标题") << QStringLiteral("发布时间"));
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // 设置输入框默认值
    ui->lineEdit->setText("10");
    ui->lineEdit->setPlaceholderText("请输入数量(1-100)");

    // 优化按钮文字
    ui->pushButton->setText(" 开始爬取");
    ui->pushButton->setMinimumWidth(150);

    // 尝试加载logo：优先使用项目根目录的 logo.png 或 source.*，否则递归搜索项目根目录/工作区根目录中的图像文件
    QString appDir = QApplication::applicationDirPath();
    QString buildRoot = QFileInfo(appDir).absolutePath() + "/../../..";
    QString workspaceRoot = QFileInfo(appDir).absolutePath() + "/../../../../";
    QStringList candidateFiles;
    candidateFiles << QFileInfo(appDir).absolutePath() + "/logo.png";
    candidateFiles << QFileInfo(appDir).absolutePath() + "/logo.jpg";
    candidateFiles << QFileInfo(appDir).absolutePath() + "/logo.jpeg";
    candidateFiles << QFileInfo(appDir).absolutePath() + "/../logo.png";
    candidateFiles << QFileInfo(appDir).absolutePath() + "/../logo.jpg";
    candidateFiles << QFileInfo(appDir).absolutePath() + "/../logo.jpeg";
    candidateFiles << QFileInfo(appDir).absolutePath() + "/../../logo.png";
    candidateFiles << QFileInfo(appDir).absolutePath() + "/../../logo.jpg";
    candidateFiles << QFileInfo(appDir).absolutePath() + "/../../logo.jpeg";
    candidateFiles << QFileInfo(appDir).absolutePath() + "/../../source/C19176DE0E593D54E9C80E03814_9A6ED23F_209A7.jpg";
    candidateFiles << QFileInfo(appDir).absolutePath() + "/../../../source/C19176DE0E593D54E9C80E03814_9A6ED23F_209A7.jpg";
    candidateFiles << QFileInfo(appDir).absolutePath() + "/../../../../source/C19176DE0E593D54E9C80E03814_9A6ED23F_209A7.jpg";

    QDirIterator it(buildRoot, QStringList() << "*.png" << "*.jpg" << "*.jpeg", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        candidateFiles << it.next();
    }

    QDirIterator it2(workspaceRoot, QStringList() << "*.png" << "*.jpg" << "*.jpeg", QDir::Files, QDirIterator::Subdirectories);
    while (it2.hasNext()) {
        candidateFiles << it2.next();
    }

    QString logoPath;
    for (const QString &path : candidateFiles) {
        if (QFile::exists(path)) {
            logoPath = path;
            break;
        }
    }
    if (!logoPath.isEmpty()) {
        QPixmap pix(logoPath);
        if (!pix.isNull()) {
            ui->label->setPixmap(pix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->label->setText("");
        }
    } else {
        ui->label->setText(QStringLiteral("LOGO"));
        ui->label->setAlignment(Qt::AlignCenter);
    }
}

void MyWidget::onStartSpider()
{
    bool ok;
    int count = ui->lineEdit->text().toInt(&ok);
    
    if (!ok || count <= 0) {
        QMessageBox::warning(this, "警告", "请输入有效的爬取条数！");
        return;
    }
    
    if (count > 100) {
        QMessageBox::warning(this, "警告", "爬取条数不能超过100条！");
        return;
    }
    
    // 清空表格
    ui->tableWidget->setRowCount(0);
    m_newsList.clear();
    
    // 禁用按钮
    ui->pushButton->setEnabled(false);
    ui->pushButton->setText("正在调用Python爬虫...");
    
    // 调用Python爬虫
    callPythonSpider(count);
}

void MyWidget::callPythonSpider(int count)
{
    // 获取Python脚本路径（相对于项目根目录）
    QString appDir = QApplication::applicationDirPath();
    
    // 尝试多个可能的路径
    QStringList possiblePaths;
    possiblePaths << appDir + "/../News_spider.py";           // build/../News_spider.py
    possiblePaths << appDir + "/../../News_spider.py";        // build/Debug/../News_spider.py
    possiblePaths << "D:/Mycode/Python/HNEU-NewsViewer-Qt/News_spider.py";  // 绝对路径
    
    QString pythonScriptPath;
    QString csvFilePath;
    bool found = false;
    
    for (const QString &path : possiblePaths) {
        if (QFile::exists(path)) {
            pythonScriptPath = path;
            // CSV文件在同一目录下
            csvFilePath = QFileInfo(pythonScriptPath).absolutePath() + "/湖南工程学院校园动态.csv";
            found = true;
            qDebug() << "找到Python脚本:" << pythonScriptPath;
            break;
        }
    }
    
    if (!found) {
        QMessageBox::critical(this, "错误", 
                              QString("未找到Python爬虫脚本！\n请确保 News_spider.py 在项目根目录下。\n当前搜索路径:\n%1")
                              .arg(possiblePaths.join("\n")));
        ui->pushButton->setEnabled(true);
        ui->pushButton->setText(" 开始爬取");
        return;
    }
    
    // 启动Python进程
    QProcess *pythonProcess = new QProcess(this);
    
    // 尝试使用虚拟环境中的Python（从项目根目录开始查找）
    QStringList venvPaths;
    venvPaths << appDir + "/../../.venv/Scripts/python.exe";       // build/Desktop_.../../.venv
    venvPaths << appDir + "/../../../.venv/Scripts/python.exe";   // build/Desktop_.../Debug/../.venv
    venvPaths << "D:/Mycode/Python/HNEU-NewsViewer-Qt/.venv/Scripts/python.exe";  // 绝对路径
    venvPaths << "D:/Mycode/Python/HNEU-NewsViewer-Qt/.venv/python.exe";          // Linux/Mac风格
    
    bool foundVenv = false;
    for (const QString &path : venvPaths) {
        if (QFile::exists(path)) {
            pythonProcess->setProgram(path);
            qDebug() << "使用虚拟环境Python:" << path;
            foundVenv = true;
            break;
        }
    }
    
    if (!foundVenv) {
        // 如果找不到虚拟环境，使用系统Python
        pythonProcess->setProgram("python");
        qDebug() << "使用系统Python";
        qDebug() << "已搜索的虚拟环境路径:";
        for (const QString &path : venvPaths) {
            qDebug() << " -" << path << "(存在:" << QFile::exists(path) << ")";
        }
    }
    
    // 强制 Python 输出 UTF-8，避免 GBK 编码失败
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PYTHONIOENCODING", "utf-8");
    pythonProcess->setProcessEnvironment(env);
    pythonProcess->setWorkingDirectory(QFileInfo(pythonScriptPath).absolutePath());
    pythonProcess->setProcessChannelMode(QProcess::MergedChannels);
    pythonProcess->setArguments(QStringList() << QStringLiteral("-u") << pythonScriptPath << QString::number(count));
    
    // 防止标准输出缓冲区填满，及时读取输出数据
    connect(pythonProcess, &QProcess::readyReadStandardOutput, this, [pythonProcess]() {
        pythonProcess->readAllStandardOutput();
    });
    
    // 启动Python进程
    pythonProcess->start();
    
    // 等待进程启动
    if (!pythonProcess->waitForStarted(5000)) {
        QMessageBox::critical(this, "错误", "无法启动Python爬虫，请确保已安装Python并配置环境变量");
        ui->pushButton->setEnabled(true);
        ui->pushButton->setText(" 开始爬取");
        delete pythonProcess;
        return;
    }
    
    // 显示进度信息
    ui->pushButton->setText("Python爬虫运行中...");
    
    // 等待Python爬虫完成
    connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, pythonProcess, csvFilePath](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus != QProcess::NormalExit || exitCode != 0) {
            QMessageBox::critical(this, "错误", 
                                  QString("Python爬虫执行失败！\n退出代码: %1\n错误信息: %2")
                                  .arg(exitCode).arg(QString::fromUtf8(pythonProcess->readAllStandardError())));
            ui->pushButton->setEnabled(true);
            ui->pushButton->setText(" 开始爬取");
            pythonProcess->deleteLater();
            return;
        }
        
        // 读取CSV文件
        QList<QMap<QString, QString>> newsList = parseCSVFile(csvFilePath);
        
        if (newsList.isEmpty()) {
            QMessageBox::warning(this, "警告", "未找到爬取的新闻数据，请检查CSV文件");
            ui->pushButton->setEnabled(true);
            ui->pushButton->setText(" 开始爬取");
            pythonProcess->deleteLater();
            return;
        }
        
        // 更新UI
        onSpiderFinished(newsList);
        pythonProcess->deleteLater();
    });
}

QList<QMap<QString, QString>> MyWidget::parseCSVFile(const QString &filePath)
{
    QList<QMap<QString, QString>> newsList;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开CSV文件:" << filePath;
        return newsList;
    }
    
    // 读取整个文件，使用状态机解析CSV，支持字段内包含换行和双引号转义
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString all = in.readAll();
    file.close();

    // 规范化换行，统一为 '\n'
    all.replace("\r\n", "\n");
    all.replace('\r', "\n");

    QList<QStringList> records;
    QString curField;
    QStringList curRecord;
    bool inQuotes = false;

    for (int i = 0; i < all.length(); ++i) {
        QChar ch = all.at(i);
        if (ch == '"') {
            // 处理双引号转义（"" -> "）
            if (i + 1 < all.length() && all.at(i + 1) == '"') {
                curField.append('"');
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (ch == ',' && !inQuotes) {
            curRecord.append(curField);
            curField.clear();
        } else if (ch == '\n' && !inQuotes) {
            curRecord.append(curField);
            curField.clear();
            records.append(curRecord);
            curRecord.clear();
        } else {
            curField.append(ch);
        }
    }
    // 处理文件末尾可能没有换行的最后一行
    if (!curField.isEmpty() || !curRecord.isEmpty()) {
        curRecord.append(curField);
        records.append(curRecord);
    }

    if (records.size() <= 1) {
        qDebug() << "CSV 文件中没有数据记录";
        return newsList;
    }

    // 第0行为表头，从第1行开始是数据
    for (int r = 1; r < records.size(); ++r) {
        QStringList rec = records.at(r);
        // 对每个字段做 trim 和 去除外围引号
        for (QString &f : rec) {
            f = f.trimmed();
            if (f.startsWith('"') && f.endsWith('"') && f.length() >= 2) {
                f = f.mid(1, f.length() - 2);
            }
        }
        if (rec.size() < 7) continue;
        QMap<QString, QString> news;
        news["title"] = rec.value(0);
        news["author"] = rec.value(1);
        news["photographer"] = rec.value(2);
        news["source"] = rec.value(3);
        news["editor"] = rec.value(4);
        news["time"] = rec.value(5);
        news["content"] = rec.value(6);
        newsList.append(news);
    }

    qDebug() << "从CSV文件读取了" << newsList.size() << "条新闻";
    return newsList;
}

void MyWidget::onProgressUpdated(int current, int total, const QString &title)
{
    // 这个函数不再使用，因为现在是批量读取CSV
}

void MyWidget::onSpiderFinished(const QList<QMap<QString, QString>> &newsList)
{
    // 保存新闻列表
    m_newsList = newsList;
    
    // 填充表格
    ui->tableWidget->setRowCount(newsList.size());
    
    for (int i = 0; i < newsList.size(); ++i) {
        const auto &news = newsList[i];
        
        // 序号
        QTableWidgetItem *itemIndex = new QTableWidgetItem(QString::number(i + 1));
        itemIndex->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 0, itemIndex);
        
        // 标题
        QTableWidgetItem *itemTitle = new QTableWidgetItem(news.value("title", "无标题"));
        itemTitle->setToolTip(news.value("title", "无标题"));
        ui->tableWidget->setItem(i, 1, itemTitle);
        
        // 时间
        QTableWidgetItem *itemTime = new QTableWidgetItem(news.value("time", "未知"));
        itemTime->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, 2, itemTime);
    }
    
    // 尝试把第一个新闻的来源信息显示到顶部 logo 区域，如果没有图片，则显示来源标签
    if (!newsList.isEmpty()) {
        QString sourceText = newsList.first().value("source", "").trimmed();
        if (!sourceText.isEmpty()) {
            const QPixmap currentPixmap = ui->label->pixmap();
            if (currentPixmap.isNull()) {
                ui->label->setText(sourceText);
                ui->label->setStyleSheet("font-weight:bold; color:#4CAF50;");
            }
        }
    }

    // 恢复按钮状态
    ui->pushButton->setEnabled(true);
    ui->pushButton->setText(" 开始爬取");
    
    QMessageBox::information(this, "成功", 
                             QString("爬取完成！共获取 %1 条新闻。\n双击表格行可查看新闻详情。").arg(newsList.size()));
}

void MyWidget::onTableDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    
    if (row < 0 || row >= m_newsList.size()) {
        return;
    }
    
    // 显示新闻详情对话框
    NewsDetailDialog *dialog = new NewsDetailDialog(m_newsList[row], this);
    dialog->exec();
    delete dialog;
}

void MyWidget::onErrorOccurred(const QString &errorMsg)
{
    ui->pushButton->setEnabled(true);
    ui->pushButton->setText("开始爬取");
    
    QMessageBox::critical(this, "错误", QString("爬取失败：%1").arg(errorMsg));
}
