#pragma once
#include "MatCpp.h"

//#include "IMU.h"
#define a 6378137.0
#define b 6356752.3142
#define f 1.0/298.257223563
#define e2 0.00669437999013
#define omega 7.2921151467e-5
#define GM 398600.4418e+9
#define ga 9.7803267714
#define gb 9.8321863685

using namespace CppToolBox;

Vector getQ(double q0, double p, const Vector & q123);
double getRn(double B);
double getRm(double B);
Vector QuaternionMultiply(const Vector & p, const Vector & q);
Vector QuaternionMultiplyAPB(const Vector & x, const Vector & y, const Vector & z);
Vector EulerAngleToQuaternion(double * in);
Vector QuaternionNorm(const Vector & obj);
Matrix QuaternionToDCM(const Vector & q);
Vector DCMToEulerAngle(Matrix & dcm);
Vector QuaternionToEulerAngle(const Vector & p);
Matrix generateAntisymmetricMatrix(const Vector & obj);
double getG(double B, double H);