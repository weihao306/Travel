#include "user.h"
#include "log.h"

User::User(int id, QDateTime startTime, QDateTime endTime, int strategy, int origin,
           int destination, bool check, std::vector<bool> throughCity) :
    check(check), id(id), strategy(strategy), origin(origin), destination(destination),
    startTime(startTime), endTime(endTime), usedTime(QDateTime(QDate(1, 1, 1), QTime(0, 0, 0, 0))),
    throughCity(throughCity), time(12, QDateTime(QDate(7999, 12, 31), QTime(23, 59, 59))),
    min_cost(0x7FFFFFFF), min_time(QDateTime(QDate(7999, 12, 31), QTime(23, 59, 59)))       //初始化变量
{
    std::vector<bool> visited(12, false);  //标记每个点是否被访问过
    //不同的旅行策略选择不同的算法
    if(strategy == 2 || check)
    {
        std::vector<QDateTime> temp_time(12, QDateTime(QDate(7999, 12, 31), QTime(23, 59, 59)));
        //临时记录每个点的到达时间
        std::vector<int> temp_cost(12);      //临时记录花费
        std::vector<Route> path;         //记录每个点的移动路径
        temp_time[origin] = startTime;   //
        DFS(origin, path, visited, temp_time, temp_cost);
    }
    else
    {
        plan = Dijkstra(visited, time);
    }
    //计算行程总时间
    totalTime = TotalDateTime();
}

//根据行程起始时间和终止时间计算总时间
QDateTime User::TotalDateTime()
{
    QDateTime endTime = time[destination];
    int used_min = (endTime.time().minute() - startTime.time().minute());
    int used_hour = (endTime.time().hour() - startTime.time().hour() - (int)((used_min >= 0) ? 0 : 1));
    int used_day = (endTime.date().day() - startTime.date().day() - (int)((used_hour >= 0) ? 0 : 1) + startTime.date().daysInMonth())
                   % startTime.date().daysInMonth();
    used_min = (used_min + 60) % 60;
    used_hour = (used_hour + 24) % 24;
    return QDateTime(QDate(1, 1, used_day + 1), QTime(used_hour, used_min, 0));
}

//获得plan的实例
std::vector<Route> User::GetPlan()
{
    return this->plan;
}

//改变计划
std::vector<Route> User::ChangePlan(int city, int strategy, int destination, QDateTime endTime,
                                    bool check, std::vector<bool> throughCity)
{
    //DFS记录关键信息的初始化
    min_cost = 0x7FFFFFFF;
    min_time = QDateTime(QDate(7999, 12, 31), QTime(23, 59, 59));
    //对旅客信息进行更改
    qDebug() << "in changePlan" << city << strategy << destination << endTime;
    std::vector<bool> visited(12, false);  //标记每个点是否被访问过
    std::vector<Route> old_plan = plan;
    std::vector<Route> temp_plan = plan;
    int old_origin = origin;
    QDateTime oldStartTime = startTime;
    origin = -1;
    //将oldplan里未走的路径删除
    for(auto iter = old_plan.begin(); iter != old_plan.end(); iter++)
    {
        if (iter->from == city)
        {
            {
                origin = iter->from;
                if (iter == old_plan.begin())
                {
                    int Min = usedTime.time().minute() + startTime.time().minute();
                    int Hour = usedTime.time().hour() + startTime.time().hour() + Min / 60;
                    int Day = usedTime.date().day() - 1 + startTime.date().day() + Hour / 24;
                    int Month = usedTime.date().month() - 1 + startTime.date().month() + Day / startTime.date().daysInMonth();
                    Min %= 60;
                    Hour %= 24;
                    Day %= startTime.date().daysInMonth();
                    startTime = QDateTime(QDate(startTime.date().year(), Month, Day), QTime(Hour, Min, startTime.time().second()));
                }
                else
                {
                    startTime = time[iter->from];
                }
                old_plan.erase(iter, old_plan.end());
            }
            break;
        }
        visited[iter->from] = true;
        throughCity[iter->from] = false;
    }
    //如果origin未变化，即city为终点城市，说明在最后一条路径，此时不能改变计划
    if (origin == -1)
    {
        std::vector<Route> nullPlan; //得到一个空路径，返回给widget弹出“无有效路径”
        plan = temp_plan;
        origin = old_origin;
        totalTime = TotalDateTime();
        return nullPlan;
    }
    plan.clear();
    this->strategy = strategy;
    this->destination = destination;
    this->endTime = endTime;
    this->check = check;
    this->throughCity = throughCity;
    //调用DFS得到新plan
    std::vector<QDateTime> temp_time(12, QDateTime(QDate(7999, 12, 31), QTime(23, 59, 59)));
    std::vector<int> temp_cost(12);
    std::vector<Route> path;     //记录每个点的移动路径
    for (auto iter = old_plan.begin(); iter != old_plan.end(); iter++)
    {
        temp_time[iter->from] = time[iter->from];
    }
    temp_time[origin] = startTime;
    DFS(origin, path, visited, temp_time, temp_cost);
    //新旧plan组合
    old_plan.insert(old_plan.end(), plan.begin(), plan.end());
    startTime = oldStartTime;
    //判断新plan是否为空且city不为目的地城市，则说明无有效路径
    if (plan.size() == 0 && city != destination)
    {
        std::vector<Route> nullPlan; //其实就是得到一个空路径，好返回给widget进入弹出“无有效路径”
        plan = temp_plan;
        origin = old_origin;
        totalTime = TotalDateTime();
        return nullPlan;
    }
    else
    {
        plan = old_plan;
        origin = old_origin;
        totalTime = TotalDateTime();
        return plan;
    }
}

//获得城市的到达时间
QDateTime User::CityArrivalTime(int index)
{
    return time[index];
}

//获得城市的离开时间
QDateTime User::CityDepartureTime(int index)
{
    std::vector<Route>::size_type id = 0;
    QTime tempTime;
    for(; id != plan.size(); id++)
        if(plan[id].from == index)
        {
            tempTime = plan[id].begin; //获得当前城市的出发时间
            break;
        }
    QDateTime DepartureDateTime = time[index];
    if(DepartureDateTime.time() <= tempTime)
    {
        return QDateTime(DepartureDateTime.date(), tempTime);
    }
    else
    {
        return QDateTime(DepartureDateTime.date().addDays(1), tempTime);
    }
}

//递归求路径
void User::DFS(int city, std::vector<Route>& path, std::vector<bool>& visited,
               std::vector<QDateTime>& temp_time, std::vector<int>& temp_cost)
{
    //按照各个策略进行剪枝
    if (strategy == 2 && (temp_time[city] > endTime || temp_cost[city] > min_cost)) //总时间大于截至时间，不满足约束条件
    {
        return;
    }
    if (strategy == 1 && temp_time[city] > min_time)
    {
        return;
    }
    if (strategy == 0 && temp_cost[city] > min_cost)
    {
        return;
    }
    visited[city] = true; //标记此城市已访问过
    //已经递归到目的地城市
    if (city == destination)
    {
        int ok = true;
        std::vector<bool> mark = throughCity;
        //将路径上的所有城市取消标志
        for (std::vector<Route>::size_type ix = 0; ix != path.size(); ix++)
        {
            mark[path[ix].to] = false;
        }
        mark[origin] = false;
        //若必经城市还有点未取消标志，则有必经城市未经过，不符合条件
        if (check)
        {
            for (std::vector<bool>::size_type ix = 0; ix != mark.size(); ix++)
            {
                if (mark[ix] == true)
                {
                    ok = false;
                    break;
                }
            }
        }
        //若满足约束条件，则更新最小值并记录路径
        if(strategy != 1 && temp_cost[city] < min_cost && ok)
        {
            min_cost = temp_cost[city];
            time = temp_time;
            plan = path;
        }
        else if (strategy == 1 && temp_time[city] < min_time && ok)
        {
            min_time = temp_time[city];
            time = temp_time;
            plan = path;
        }
    }
    //未到达目的地城市，继续向下一层递归
    else
    {
        typedef std::multimap<int, Route>::size_type sz_type;
        sz_type entries = Schedule::data.count(city);
        auto iter = Schedule::data.find(city);
        auto min = iter;
        bool start = true;
        for(sz_type cnt = 0; cnt != entries; cnt++, iter++)
        {
            if (visited[iter->second.to] == true) //如果去往城市已经访问过，则忽略该路径
            {
                if (start)
                {
                    min++;
                }
                continue;
            }
            start = false;
            if (strategy != 2)
            {
                //找到一条通往新的城市的路径
                if (iter->second.to != min->second.to)
                {
                    if (!visited[min->second.to])
                    {
                        path.push_back(min->second);
                        temp_time[min->second.to] = CalculateTime(min, temp_time);
                        temp_cost[min->second.to] = temp_cost[city] + min->second.cost;
                        DFS(min->second.to, path, visited, temp_time, temp_cost);
                        //回溯状态
                        visited[min->second.to] = false;
                        path.erase(path.end());
                        //更新min
                        min = iter;
                    }
                }
                //此路径的去往城市跟之前相同，则寻找最小值
                else
                {
                    if (strategy == 0 && iter->second.cost < min->second.cost)
                    {
                        min = iter;
                    }
                    if (strategy == 1 && CalculateTime(iter, temp_time) < CalculateTime(min, temp_time))
                    {
                        min = iter;
                    }
                }
            }
            //策略三不能局部最优剪枝，所以每条路径都需要尝试
            else
            {
                path.push_back(iter->second);
                //更新时间和花费
                temp_time[iter->second.to] = CalculateTime(iter, temp_time);
                temp_cost[iter->second.to] = temp_cost[city] + iter->second.cost;
                DFS(iter->second.to, path, visited, temp_time, temp_cost);
                visited[iter->second.to] = false;
                path.erase(path.end());
            }
        }
        //循环结束后还需要处理一次min
        if (strategy != 2)
        {
            if (!visited[min->second.to])
            {
                path.push_back(min->second);
                temp_time[min->second.to] = CalculateTime(min, temp_time);
                temp_cost[min->second.to] = temp_cost[city] + min->second.cost;
                DFS(min->second.to, path, visited, temp_time, temp_cost);
                visited[min->second.to] = false;
                path.erase(path.end());
            }
        }
    }
}

void User::UpdateAdjacents(int city, std::vector<int>& value, std::vector<bool>& visited, std::vector<Route>& path)
{
    typedef std::multimap<int, Route>::size_type sz_type;
    sz_type entries = Schedule::data.count(city);
    std::multimap<int, Route>::iterator iter = Schedule::data.find(city);
    for(sz_type cnt = 0; cnt != entries; cnt++, iter++)
    {
        //判断是否时间跨天
        bool span;
        if(iter->second.begin <= iter->second.end)
        {
            span = false;
        }
        else
        {
            span = true;
        }
        if(strategy == 0)
        {
            //策略一:花费最少
            if(!visited[iter->second.to] && value[iter->second.to] > value[city] + iter->second.cost)
            {
                value[iter->second.to] = value[city] + iter->second.cost;
                path[iter->second.to] = iter->second;
                //更新
                if(!span && time[iter->second.from].time() <= iter->second.begin)
                {
                    //列车行程不跨天，到达城市的时间在列车出发时间之前
                    time[iter->second.to] = QDateTime(time[iter->second.from].date(), iter->second.end);
                    //到达下一个城市时间=当前城市的日期+列车到达时间
                }
                else if(!span && time[iter->second.from].time() > iter->second.begin)
                {
                    //列车行程不跨天，到达城市的时间在列车出发时间之后
                    time[iter->second.to] = QDateTime(time[iter->second.from].date().addDays(1), iter->second.end);
                    //到达下一个城市的时间=当前城市的下一天日期+列车到达时间
                }
                else if(span && time[iter->second.from].time() <= iter->second.begin)
                {
                    //列车行程跨天，到达城市的时间在列车出发时间之前
                    time[iter->second.to] = QDateTime(time[iter->second.from].date().addDays(1), iter->second.end);
                    //到达下一个城市的时间=当前城市的下一天日期+列车到达时间
                }
                else if(span && time[iter->second.from].time() > iter->second.begin)
                {
                    //列车行程跨天，到达城市的时间在列车出发时间之后
                    time[iter->second.to] = QDateTime(time[iter->second.from].date().addDays(2), iter->second.end);
                    //到达下一个城市的时间=当前城市后天的日期+列车到达时间
                }
            }
        }
        else if(strategy == 1)
        {
            //策略二:时间最短
            if(!visited[iter->second.to])
            {
                if(!span && time[iter->second.from].time() <= iter->second.begin &&
                        time[iter->second.to] > QDateTime(time[iter->second.from].date(), iter->second.end))
                {
                    time[iter->second.to] = QDateTime(time[iter->second.from].date(), iter->second.end);
                    path[iter->second.to] = iter->second;
                }
                else if(!span && time[iter->second.from].time() > iter->second.begin &&
                        time[iter->second.to] > QDateTime(time[iter->second.from].addDays(1).date(), iter->second.end))
                {
                    time[iter->second.to] = QDateTime(time[iter->second.from].addDays(1).date(), iter->second.end);
                    path[iter->second.to] = iter->second;
                }
                else if(span && time[iter->second.from].time() <= iter->second.begin &&
                        time[iter->second.to] > QDateTime(time[iter->second.from].addDays(1).date(), iter->second.end))
                {
                    time[iter->second.to] = QDateTime(time[iter->second.from].addDays(1).date(), iter->second.end);
                    path[iter->second.to] = iter->second;
                }
                else if(span && time[iter->second.from].time() > iter->second.begin &&
                        time[iter->second.to] > QDateTime(time[iter->second.from].addDays(2).date(), iter->second.end))
                {
                    time[iter->second.to] = QDateTime(time[iter->second.from].addDays(2).date(), iter->second.end);
                    path[iter->second.to] = iter->second;
                }
            }
        }
    }
}

std::vector<Route> User::Dijkstra(std::vector<bool> &visited, std::vector<QDateTime>& time)
{
    std::vector<int> cost(12, INT_MAX); //记录原点到每个点的权值之和
    std::vector<Route> path(12);     //记录每个点的移动路径
    QDateTime currentTime = startTime;
    time[origin] = currentTime;
    visited[origin] = true;
    cost[origin] = 0;
    int city = origin;
    while(true)
    {
        //更新相邻节点的值
        UpdateAdjacents(city, cost, visited, path);
        //qDebug() << "update success...";
        city = -1;
        if(strategy == 0)
        {
            int min = INT_MAX;
            //寻找未访问点中最小的权值
            for(std::vector<int>::size_type ix = 0;
                    ix != cost.size(); ix++)
            {
                if(!visited[ix] && min > cost[ix])
                {
                    min = cost[ix];
                    city = ix;
                    //qDebug() << city;
                }
            }
        }
        else if(strategy == 1)
        {
            QDateTime minn(QDate(7999, 12, 31), QTime(23, 59, 59));
            for(std::vector<QDateTime>::size_type ix = 0;
                    ix != time.size(); ix++)
            {
                if(!visited[ix] && minn > time[ix])
                {
                    minn = time[ix];
                    city = ix;
                    //qDebug() << city;
                }
            }
        }
        if(city == -1)
        {
            break;
        }
        visited[city] = true;
    }
    qDebug() << "Loop finish...";
    std::vector<Route> plan;
    MakePlan(destination, path, plan);
    qDebug() << "Makeplan finish...";
    return plan;
}

QDateTime User::CalculateTime(const std::multimap<int, Route>::iterator& iter,
                              std::vector<QDateTime>& tempTime) //计算时间
{
    QDateTime temp;
    //判断是否时间跨天
    bool span;
    if(iter->second.begin <= iter->second.end)
    {
        span = false;
    }
    else
    {
        span = true;
    }
    if(!span && tempTime[iter->second.from].time() <= iter->second.begin)
    {
        temp = QDateTime(tempTime[iter->second.from].date(), iter->second.end);
    }
    else if(!span && tempTime[iter->second.from].time() > iter->second.begin)
    {
        temp = QDateTime(tempTime[iter->second.from].date().addDays(1), iter->second.end);
    }
    else if(span && tempTime[iter->second.from].time() <= iter->second.begin)
    {
        temp = QDateTime(tempTime[iter->second.from].date().addDays(1), iter->second.end);
    }
    else if(span && tempTime[iter->second.from].time() > iter->second.begin)
    {
        temp = QDateTime(tempTime[iter->second.from].date().addDays(2), iter->second.end);
    }
    return temp;
}

//逆向path生成plan
void User::MakePlan(int city, const std::vector<Route>& path, std::vector<Route> &plan)
{
    if(path[city].from == -1)
    {
        return;
    }
    if(city != origin)
    {
        MakePlan(path[city].from, path, plan);
        plan.push_back(path[city]);
        qDebug() << path[city].from << path[city].num << path[city].to;
    }
}
