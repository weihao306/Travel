#include "subwidget.h"

#include <QApplication>
#include <QStateMachine>
#include <QPushButton>
#include <QSignalTransition>
#include <QPropertyAnimation>
#include <QPainter>
#include <QState>
#include <QLabel>

SubWidget::SubWidget(QWidget *parent) :
    QWidget(parent), state(-1), at()
{}


//中途更改计划时调用，用于获得新计划的始发地
int SubWidget::nextCity()
{
    Widget *fatherPtr = (Widget *)parentWidget();
    std::vector<Route> path = fatherPtr->travelers[fatherPtr->currentTraveler].GetPlan();
    int nextCity2Arrive;
    QDateTime spenttime = fatherPtr->GetSpentTime();
    //已经到达目的地，则无法改变目的地
    if(spenttime >= fatherPtr->travelers[fatherPtr->currentTraveler].totalTime)
    {
        nextCity2Arrive = -1;
        state = -2;
        at.from = -2;
    }
    else
        for (std::vector<Route>::size_type index = 0;
                index != path.size(); index++)
        {
            QDateTime starttime = fatherPtr->travelers[fatherPtr->currentTraveler].startTime;
            QDateTime departuredatetime = fatherPtr->travelers[fatherPtr->currentTraveler].CityDepartureTime(path[index].from);
            QDateTime cityarrivaltime = fatherPtr->travelers[fatherPtr->currentTraveler].CityArrivalTime(path[index].to);
            //当前处于等待状态，新计划始发地为当前所处地点
            if (spenttime <= getSplitTime(starttime, departuredatetime))
            {
                nextCity2Arrive = path[index].from;
                state = -1;
                at.from = -1;
                break;
            }
            //当前处于运行状态，新计划为即将到达的城市
            else if (spenttime <=
                     getSplitTime(starttime, cityarrivaltime))
            {
                nextCity2Arrive = path[index].to;
                state = path[index].way;
                at = path[index];
                break;
            }
        }
    qDebug() << "Next city to arrive is " << nextCity2Arrive;
    return nextCity2Arrive;
}

//获得两时间点之间的时间差，判断当前所处的状态
QDateTime SubWidget::getSplitTime(QDateTime former, QDateTime later)
{
    int durationSec = (later.time().second() - former.time().second());
    int durationMin = (later.time().minute() - former.time().minute() - (int)((durationSec >= 0) ? 0 : 1));
    int durationHour = (later.time().hour() - former.time().hour() - (int)((durationMin >= 0) ? 0 : 1));
    int durationDay = (later.date().day() - former.date().day() - (int)((durationHour >= 0) ? 0 : 1) + former.date().daysInMonth())
                      % former.date().daysInMonth();
    durationSec = (durationSec + 60) % 60;
    durationMin = (durationMin + 60) % 60;
    durationHour = (durationHour + 24) % 24;
    return QDateTime(QDate(1, 1, durationDay + 1), QTime(durationHour, durationMin, durationSec, 999));
}
