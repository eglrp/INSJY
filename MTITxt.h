#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "Structs.h"
struct Time
{
	//double nsec;  /*���� - ȡֵ����Ϊ[0,59] */
	//int usec;  /*΢�� - ȡֵ����Ϊ[0,59] */
	//int msec;  /*���� - ȡֵ����Ϊ[0,59] */
	double sec;     /*�� - ȡֵ����Ϊ[0,59] */
	int min;     /*�� - ȡֵ����Ϊ[0,59] */
	int hour;    /*ʱ - ȡֵ����Ϊ[0,23] */
	int day;     /*�� - ȡֵ����Ϊ[1,31] */
	int mon;     /*�� - ȡֵ����Ϊ[1,12] */
	int year;    /*�� - ȡֵ����>1980 ��*/

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

	//Ϊ������㽫��monthת��
	int m = month;
	if (m <= 2)
	{
		m += 12;
		y -= 1;
	}

	int e = (int)(30.6 * (m + 1));
	int a = (int)(y / 100);

	// �̻ʸ��׸���ʮ������1582��2��24���Խ̻�ѵ��䲼����1582��10��5����14��Ĩ��
	//1582��10��4�չ����ڶ�����10��15��
	int b;
	if ((year < 1582) || (year == 1582 && month < 10) || (year == 1582 && month == 10 && day < 15))
	{
		b = -38;
	}
	else
	{
		b = (int)(a / 4 - a);
	}

	//����������
	int c = (int)(365.25 * y);
	double jd = b + c + e + day - 32167.5;
	mjd = jd - 2400000.5;
	return mjd;
}

void UTCTime2GPSTime()
{
	double mjd = UTCTime2MJDTime();
	//1980��1��6��0ʱ0��0�� == MJD- 44244
	double temp = mjd - 44244;

	//����������ĵ�ת��Ϊ��ݣ���1988��6��30��23��59��60��,
	//��ô����6��֮ǰ���벻�䣬6��֮�������һ��
	//�����·ݼ���ָ��x����6/12<x<7/12��������ʱ����,
	//��12��31�����ӵ����뷽������
	double TransitYear[] = { 1981.58, 1982.58, 1983.58, 1985.58, 1988.08, 1990.08, 1991.08, 1992.58, 1993.58,
		1994.58, 1996.08, 1997.58, 1999.08, 2006.08, 2009.08, 2012.58 };
	int LeapSecond = 0; //����
	double TempYear = time.year + time.mon / 12.0;  //utcʱ����ݣ����·ݰ�����ȥ
	int length = sizeof(TransitYear) / sizeof(double);
	for (int i = 0; i < length; i++)
	{
		LeapSecond += TempYear / TransitYear[i];
	}


	time.weekID = (int)(temp / 7); //����
	temp = temp - time.weekID * 7;       //��������
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