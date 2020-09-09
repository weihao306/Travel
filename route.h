#ifndef ROUTE_H
#define ROUTE_H

#include <QString>
#include <QTime>

class Route   //路线
{
public:
    Route();
    Route(int, int, QString, QTime, QTime, int, int);   //存储一条列车/航班的信息

    int from, to, cost, way;    //出发地、目的地、花费、方式(0汽车、1火车、2飞机)
    QString num;    //列车or航班班次
    QTime begin, end;   //出发时间、到达时间

};

#endif // ROUTE
