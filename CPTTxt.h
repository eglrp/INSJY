#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "Structs.h"
const char imu_data_flag[] = "%RAWIMUSA";
void cpt_txt_input_data(FILE * fp, INSData * out)
{
	char temp_header[40];
	char temp_body[200];
	char rab[20];
	fscanf(fp, "%s,%s\n", temp_header, temp_body);
	while (strcmp(imu_data_flag, temp_header) != 0 && !feof(fp))
	{
		fscanf(fp, "%s,%s\n", temp_header, temp_body);
	}
	
	double mweek, msec, sweek, imu_status;
	sscanf(temp_body, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf*%s",
		&mweek,&msec,&sweek,
		&out->Time,
		&imu_status,
		out->A,out->A + 1,out->A + 2,
		out->G,out->G + 1,out->G + 2,
		rab);

}