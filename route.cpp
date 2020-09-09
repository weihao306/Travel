#include "route.h"
#include "log.h"

Route::Route()
{
    this->from = -1;
}

Route::Route(int from, int to, QString num, QTime begin, QTime end, int cost, int way)
{
    this->from = from;
    this->to = to;
    this->num = num;
    this->begin = begin;
    this->end = end;
    this->cost = cost;
    this->way = way;
}



