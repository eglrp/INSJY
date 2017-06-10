#pragma once
#include <stdio.h>
#include "Structs.h"
class ResultFile {
public:
	static ResultFile * create(const char * file)
	{
		ResultFile * resultFile = new ResultFile();
		resultFile->fp = fopen(file, "w");
		fprintf(resultFile->fp, "Time\tMotion_X\tMotion_Y\tMotion_Z\tVelocity_X\tVelocity_Y\tVelocity_Z\tPosition_X\tPosition_Y\tPosition_Z\n");
	}
	void append(INSResult * result)
	{
		fprintf(fp, "%.10lf\t%.10lf\t%.10lf\t%.10lf\t%.10lf\t%.10lf\t%.10lf\t%.10lf\t%.10lf\t%.10lf\n",
			&result->Time,
			result->Motion, result->Motion + 1, result->Motion + 2,
			result->Velocity, result->Velocity + 1, result->Velocity + 2,
			result->Position, result->Position + 1, result->Position + 2);
	}
	void close()
	{
		fclose(fp);
	}
private:
	ResultFile() :fp(NULL) {}
	FILE * fp;
};