#include "MyNotepad.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QDir>

// 全局异常处理函数
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);  // 标记未使用的参数

    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtInfoMsg:
        txt = QString("Info: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        break;
    }

    // 创建日志目录
    QDir dir;
    if (!dir.exists("logs")) {
        dir.mkdir("logs");
    }

    // 写入日志文件
    QFile outFile("logs/crash.log");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " - " << txt << "\n";
    outFile.close();

    // 如果是致命错误，显示错误对话框
    if (type == QtFatalMsg) {
        QMessageBox::critical(nullptr, "程序错误",
            QString("程序发生严重错误：\n%1\n\n"
                   "错误已记录到日志文件：logs/crash.log\n"
                   "请将日志文件发送给开发人员以帮助解决问题。").arg(msg));
    }
}

int main(int argc, char *argv[])
{
    // 安装消息处理器
    qInstallMessageHandler(customMessageHandler);

    try {
        QApplication a(argc, argv);
        MyNotepad w;
        w.show();
        return a.exec();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "程序异常",
            QString("程序发生异常：\n%1\n\n"
                   "异常已记录到日志文件：logs/crash.log\n"
                   "请将日志文件发送给开发人员以帮助解决问题。").arg(e.what()));
        return 1;
    } catch (...) {
        QMessageBox::critical(nullptr, "程序异常",
            "程序发生未知异常\n\n"
            "异常已记录到日志文件：logs/crash.log\n"
            "请将日志文件发送给开发人员以帮助解决问题。");
        return 1;
    }
}
