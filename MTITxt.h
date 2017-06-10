#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "Structs.h"
struct Time
{
	//double nsec;  /*纳秒 - 取值区间为[0,59] */
	//int usec;  /*微妙 - 取值区间为[0,59] */
	//int msec;  /*毫秒 - 取值区间为[0,59] */
	double sec;     /*秒 - 取值区间为[0,59] */
	int min;     /*分 - 取值区间为[0,59] */
	int hour;    /*时 - 取值区间为[0,23] */
	int day;     /*天 - 取值区间为[1,31] */
	int mon;     /*月 - 取值区间为[1,12] */
	int year;    /*年 - 取值区间>1980 年*/

	int weekID;
	double SecInWeek;
};
Time time;
double UTCTime2MJDTime()
{
	double mjd;
	int year = time.year;
	int month = time.mon;
	double day = time.day + (time.hour * 3600 + time.min * 60 + time.sec) / 86400.0;
	int y = time.year + 4800;

	if (year < 0)
		y += 1;

	//为方便计算将将month转化
	int m = month;
	if (m <= 2)
	{
		m += 12;
		y -= 1;
	}

	int e = (int)(30.6 * (m + 1));
	int a = (int)(y / 100);

	// 教皇格雷戈里十三世于1582年2月24日以教皇训令颁布，将1582年10月5日至14日抹掉
	//1582年10月4日过完后第二天是10月15日
	int b;
	if ((year < 1582) || (year == 1582 && month < 10) || (year == 1582 && month == 10 && day < 15))
	{
		b = -38;
	}
	else
	{
		b = (int)(a / 4 - a);
	}

	//计算儒略日
	int c = (int)(365.25 * y);
	double jd = b + c + e + day - 32167.5;
	mjd = jd - 2400000.5;
	return mjd;
}

void UTCTime2GPSTime()
{
	double mjd = UTCTime2MJDTime();
	//1980年1月6日0时0分0秒 == MJD- 44244
	double temp = mjd - 44244;

	//将增加闰秒的点转换为年份，如1988年6月30日23点59分60秒,
	//那么在在6月之前闰秒不变，6月之后闰秒加一，
	//故以月份计算分割点x，即6/12<x<7/12，不考虑时分秒,
	//在12月31日增加的闰秒方法类似
	double TransitYear[] = { 1981.58, 1982.58, 1983.58, 1985.58, 1988.08, 1990.08, 1991.08, 1992.58, 1993.58,
		1994.58, 1996.08, 1997.58, 1999.08, 2006.08, 2009.08, 2012.58 };
	int LeapSecond = 0; //闰秒
	double TempYear = time.year + time.mon / 12.0;  //utc时间年份，将月份包含进去
	int length = sizeof(TransitYear) / sizeof(double);
	for (int i = 0; i < length; i++)
	{
		LeapSecond += TempYear / TransitYear[i];
	}


	time.weekID = (int)(temp / 7); //周数
	temp = temp - time.weekID * 7;       //周内天数
	time.SecInWeek = temp * 86400 + LeapSecond;
	//if (time.weekID >= 1024)
	//{
	//time.weekID %= 1024;
	//}
}
void mti_txt_input_data(FILE * fp, INSData * out)
{
	char temp[300];
	fscanf(fp, "%s", temp);
	double p1, p2,nano;
	int valid;
	sscanf(temp, "%d%d%lf%d%d%d%d%d%d%d%lf%lf%lf%lf%lf%lf",
		&p1,&p2,
		&nano,
		&time.year,
		&time.mon,
		&time.day,
		&time.hour,
		&time.min,
		&time.sec,
		&valid,
		out->A, out->A + 1, out->A + 2,
		out->G, out->G + 1, out->G + 2);
	time.sec += nano / 1e9;
	UTCTime2GPSTime();
	out->Time = time.SecInWeek;
}