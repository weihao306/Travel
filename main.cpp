#include "widget.h"
#include "log.h"

#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //输出log.txt文件
    //支持中文编码
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    qInstallMessageHandler(OutputMessage);
    Widget window;
    window.show();
    return a.exec();
}
