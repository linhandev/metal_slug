#include "util.h"

int Point::get_x(){ return x; }
int Point::get_y(){ return y; }

double DP::get_x(){ return x; };
double DP::get_y(){ return y; };
void DP::set_pos(double xx, double yy)
{
    x = xx, y = yy;
}


int max(int a,int b)
{
    return a>b?a:b;
}
int min(int a,int b)
{
    return a<b?a:b;
}

int Box::get_x_low()
{
    return min(ul.get_x(), lr.get_x());
}
int Box::get_x_high()
{
    return max(ul.get_x(), lr.get_x());
}
int Box::get_y_low()
{
    return min(ul.get_y(), lr.get_y());
}
int Box::get_y_high()
{
    return max(ul.get_y(), lr.get_y());
}

int Line::get_min_x()
{
    return min(int(s.get_x()) , int(e.get_x()));
}

int Line::get_max_x()
{
    return max(int(s.get_x()), int(e.get_x()));
}

int Line::get_y()
{
    return int(s.get_y());
}
