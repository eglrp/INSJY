#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "Structs.h"
#define PI 3.1415926535897932384626433832795
void sbg_txt_input_data(FILE * fp, INSData * out)
{
	fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf\n",
		&out->Time,
		out->A, out->A + 1, out->A + 2,
		out->G, out->G + 1, out->G + 2);
	out->G[0] *= PI / 180.0;
	out->G[1] *= PI / 180.0;
	out->G[2] *= PI / 180.0;
}