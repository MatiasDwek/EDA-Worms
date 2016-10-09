#ifndef POINT_H
#define POINT_H

#define FLOAT_ERROR 0.000001

struct Point
{
    int x;
    int y;
};

struct Velocity
{
    double x;
    double y;
};

double getDistanceBetweenPoints(Point p1, Point p2);
double getAngleBetweenPoints(Point p1, Point p2); //p1 explosion, p2 worm

#endif // POINT_H
