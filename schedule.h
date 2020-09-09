#ifndef SCHEDULE_H
#define SCHEDULE_H

#include "route.h"

#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <QString>
#include <QObject>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class Schedule  //时刻表
{
public:
    Schedule();   //从文本导入班次信息
    static int CityToNum(QString);     //将城市编号，便于计算
    static std::multimap<int, Route> data;   //保存班次信息

};

#endif // SCHEDULE_H
