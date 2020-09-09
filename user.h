#ifndef TRAVELER_H
#define TRAVELER_H

#include "route.h"
#include "schedule.h"

#include <vector>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <cmath>


class User
{
public:
    bool check; //旅客是否被询问
    int id, strategy;    //旅客编号，旅行策略(0:费用最少，1:用时最短，2:双重限制)
    int origin, destination;    //出发地，目的地
    QDateTime startTime, endTime, usedTime;  //开始时间，结束时间，花费时间
    QDateTime totalTime;    //总时间
    std::vector<bool> throughCity;  //标记途径城市

    User(int, QDateTime, QDateTime, int, int, int, bool, std::vector<bool>);
    std::vector<Route> GetPlan();   //获得旅行路线
    QDateTime CityArrivalTime(int);     //到达某城市的时间点
    QDateTime CityDepartureTime(int);   //离开某城市的时间点
    std::vector<Route> ChangePlan(int, int, int, QDateTime,
                                  bool, std::vector<bool> );    //变更旅游计划


private:
    std::vector<Route> plan; //记录最优路径
    std::vector<QDateTime> time; //记录每个城市的到达时间
    std::vector<Route> Dijkstra(std::vector<bool> &, std::vector<QDateTime> &);
    void DFS(int city, std::vector<Route>&, std::vector<bool>&,
             std::vector<QDateTime>&, std::vector<int> &);  //通过深度优先遍历得到一条路线
    void MakePlan(int, const std::vector<Route>&, std::vector<Route> &);    //得到旅游规划
    void UpdateAdjacents(int, std::vector<int>&, std::vector<bool>&, std::vector<Route>&);  //更新邻接点的状态
    QDateTime CalculateTime(const std::multimap<int, Route>::iterator&,
                            std::vector<QDateTime>&);   //计算花费的时间
    QDateTime TotalDateTime();  //计算总用时

    int min_cost;   //最少花费
    QDateTime min_time; //最短用时
};

#endif // TRAVELER_H
