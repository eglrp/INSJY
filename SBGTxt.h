#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "Structs.h"
void sbg_txt_input_data(FILE * fp, INSData * out)
{
	fscanf(fp, "%lf%lf%lf%lf%lf%lf%lf",
		&out->Time,
		out->A, out->A + 1, out->A + 2,
		out->G, out->G + 1, out->G + 2);
}