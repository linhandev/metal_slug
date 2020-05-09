#ifndef SLUG_UTIL
#define SLUG_UTIL

class Point
{
    int x,y;
public:
    Point(int xx, int yy):x(xx),y(yy){}
    int get_x();
    int get_y();
    void add_x(int dist){ x = x + dist; };
    void add_y(int dist){ y = y + dist; };
    void add_x_y(int dx, int dy){ x+=dx, y+=dy; }
};
class DP
{
    double x,y;
public:
    DP():x(0),y(0){ }
    DP(double xx, double yy):x(xx), y(yy){ }
    DP(int xx, int yy):x(xx), y(yy){ }
    double get_x();
    double get_y();
    void add_x(double dist){x = x + dist; }
    void add_y(double dist){y = y + dist; }
    void add_x_y(double dx, double dy){ x+=dx,y+=dy;}
    void set_y(double yy){y=yy;}
    void set_y(int yy){y=yy; }

    void set_pos(double xx, double yy);
};

class Line
{
    Point s,e;
public:
    Line(Point ss,Point ee):s(ss),e(ee){}
    int get_min_x();
    int get_max_x();
    int get_y();
};
class Box
{
    Point ul,lr;
public:
    Box(Point a, Point b):ul(a),lr(b){ }
    int get_x_low();
    int get_x_high();
    int get_y_low();
    int get_y_high();
};

#endif
