#include "schedule.h"
#include "log.h"
std::multimap<int, Route> Schedule::data;

Schedule::Schedule()
{
    QFile file(":/data.txt");
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Could not open the file by reading";
        return;
    }
    QTextStream in(&file);
    QString from, to, number, begin, end, cost, way; //分别为出发地、目的地、班次、开始时间、结束时间、费用、交通方式
    //将时刻表信息读入data数据结构
    while(!in.atEnd())
    {
        in >> from >> to >> number >> begin >> end >> cost >> way >> endl;
        if(from != "")
        {
            //qDebug() << from << to << number << begin;
            Route line(CityToNum(from), CityToNum(to), number, QTime::fromString(begin),
                       QTime::fromString(end), cost.toInt(), way.toInt());
            data.insert(std::make_pair(CityToNum(from), line));
        }
    }
    qDebug() << "input success...";
}

//根据字符串转换为相应的序号
int Schedule::CityToNum(QString city)
{
    int num = -1;
    if(city == "Beijing" || city == "北京")
    {
        num = 0;
    }
    else if(city == "Shanghai" || city == "上海")
    {
        num = 1;
    }
    else if(city == "Chongqing" || city == "重庆")
    {
        num = 2;
    }
    else if(city == "Xi'an" || city == "西安")
    {
        num = 3;
    }
    else if(city == "Shenzhen" || city == "深圳")
    {
        num = 4;
    }
    else if(city == "Zhengzhou" || city == "郑州")
    {
        num = 5;
    }
    else if(city == "Wuhan" || city == "武汉")
    {
        num = 6;
    }
    else if(city == "Lasa" || city == "拉萨")
    {
        num = 7;
    }
    else if(city == "Haikou" || city == "海口")
    {
        num = 8;
    }
    else if(city == "Shenyang" || city == "沈阳")
    {
        num = 9;
    }
    else if(city == "lanzhou" || city == "兰州")
    {
        num = 10;
    }
    else if(city == "Kunming" || city == "昆明")
    {
        num = 11;
    }
    return num;
}
