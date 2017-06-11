#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "Structs.h"

#define GSF 1.0850694444444444444444444444444e-7
#define ASF 0.00000152587890625

const char imu_data_flag[] = "%RAWIMUSA";
void cpt_txt_input_data(FILE * fp, INSData * out)
{
	char temp_header[40];
	char temp_body[200];
	char rab[20];
	fscanf(fp, "%9s,%s\n", temp_header, temp_body);
	while (strcmp(imu_data_flag, temp_header) != 0 && !feof(fp))
	{
		fscanf(fp, "%s,%s\n", temp_header, temp_body);
	}
	
	double mweek, msec, sweek, imu_status;
	sscanf(temp_body, "%lf,%lf;%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf*%s\n",
		&mweek,&msec,&sweek,
		&out->Time,
		&imu_status,
		out->A,out->A + 1,out->A + 2,
		out->G,out->G + 1,out->G + 2,
		rab);

	out->A[0] *= ASF / 0.01;
	out->A[1] *= ASF / 0.01;
	out->A[2] *= ASF / 0.01;
	out->G[0] *= GSF / 0.01;
	out->G[1] *= GSF / 0.01;
	out->G[2] *= GSF / 0.01;
}