#include <cmath>
#include "point.h"
#include "../../Front_End/Defines/graphicdefines.h"

double getDistanceBetweenPoints(Point p1, Point p2)
{
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

double getAngleBetweenPoints(Point p1, Point p2)
{
	Point pRet;
	pRet.x = p1.x-p2.x;
	pRet.y = p1.y-p2.y;
	if (pRet.x == 0)
    {
        if (pRet.y >= 0)
            return PI / 2.0;
        else
            return -PI / 2.0;
    } else if (pRet.x<0)
	{
		pRet.x = -pRet.x;
        //pRet.y = -pRet.y;
		return PI-atan(pRet.y/pRet.x);
	}
    return atan(pRet.y/pRet.x);
}
