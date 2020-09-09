#ifndef SubWidget_H
#define SubWidget_H

#include "widget.h"

#include <QWidget>
#include <QTextEdit>
#include <QBrush>
#include <QPalette>
#include <QPixmap>
#include <QPen>
#include <QDebug>
#include <QRectF>
#include <QDateTime>
#include <QPointF>
#include <QMessageBox>
#include <QTimer>
#include <QThread>
class SubWidget : public QWidget
{
public:
    SubWidget(QWidget *);
    QDateTime getSplitTime(QDateTime former, QDateTime later);//获取两时间点时间间隔
    int nextCity(); //获得新计划的始发地
    int getState()  //获得当前乘客的状态
    {
        return state;
    }
    Route GetAt()   //得到当前乘客的班次
    {
        return at;
    }
private:
    int state;
    Route at;
};

#endif // SubWidget_H
