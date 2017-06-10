#pragma once
struct INSResult
{
	double Time, Motion[3], Velocity[3], Position[3];
};
struct INSData
{
	double Time, A[3], G[3];
};