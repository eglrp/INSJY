#include "DetailedFunctions.h"

Vector getQ(double q0, double p, const Vector & q123)
{
	Vector total = Vector(4);
	total[0] = q0;
	total[1] = p * q123[0];
	total[2] = p * q123[1];
	total[3] = p * q123[2];

	return total;
}
double getRn(double B)
{
	return a / sqrt(1 - e2 * sin(B) * sin(B));
}
double getRm(double B)
{
	return a*(1 - e2) / (sqrt((1 - e2*sin(B)*sin(B))*(1 - e2*sin(B)*sin(B))*(1 - e2*sin(B)*sin(B))));
}
Vector QuaternionMultiply(const Vector & p, const Vector & q)
{
	Vector m = Vector(4);
	m[0] = p[0] * q[0] - p[1] * q[1] - p[2] * q[2] - p[3] * q[3];
	m[1] = p[1] * q[0] + p[0] * q[1] - p[3] * q[2] + p[2] * q[3];
	m[2] = p[2] * q[0] + p[3] * q[1] + p[0] * q[2] - p[1] * q[3];
	m[3] = p[3] * q[0] - p[2] * q[1] + p[1] * q[2] + p[0] * q[3];
	return m;
}
Vector QuaternionMultiplyAPB(const Vector & x, const Vector & y, const Vector & z)
{
	Vector temp = QuaternionMultiply(x, y);
	return QuaternionMultiply(temp, z);
}
Vector EulerAngleToQuaternion(double * in)
{
	Vector total = Vector(4);
	total[0] = cos(in[0] / 2.0)*cos(in[1] / 2.0)*cos(in[2] / 2.0) + sin(in[0] / 2.0)*sin(in[1] / 2.0)*sin(in[2] / 2.0);
	total[1] = sin(in[0] / 2.0)*cos(in[1] / 2.0)*cos(in[2] / 2.0) - cos(in[0] / 2.0)*sin(in[1] / 2.0)*sin(in[2] / 2.0);
	total[2] = cos(in[0] / 2.0)*sin(in[1] / 2.0)*cos(in[2] / 2.0) + sin(in[0] / 2.0)*cos(in[1] / 2.0)*sin(in[2] / 2.0);
	total[3] = cos(in[0] / 2.0)*cos(in[1] / 2.0)*sin(in[2] / 2.0) - sin(in[0] / 2.0)*sin(in[1] / 2.0)*cos(in[2] / 2.0);
	return total;
}
Vector QuaternionNorm(const Vector & obj)
{
	Vector total = Vector(4);
	double mod = obj.mod();
	for (int i = 0;i < 4;i++)total[i] = obj[i] / mod;
	return total;
}
Matrix QuaternionToDCM(const Vector & q)
{
	Matrix total = Matrix::zeros(3, 3);
	total[0][0] = q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3];
	total[0][1] = 2 * (q[1] * q[2] - q[0] * q[3]);
	total[0][2] = 2 * (q[1] * q[3] + q[0] * q[2]);
	total[1][0] = 2 * (q[1] * q[2] + q[0] * q[3]);
	total[1][1] = q[0] * q[0] - q[1] * q[1] + q[2] * q[2] - q[3] * q[3];
	total[1][2] = 2 * (q[2] * q[3] - q[0] * q[1]);
	total[2][0] = 2 * (q[1] * q[3] - q[0] * q[2]);
	total[2][1] = 2 * (q[2] * q[3] + q[0] * q[1]);
	total[2][2] = q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3];
	return total;
}
Vector DCMToEulerAngle(Matrix & dcm)
{
	Vector angle = Vector(3);
	angle[0] = atan2(dcm[2][1], dcm[2][2]);
	angle[1] = atan2(-dcm[2][0], sqrt(dcm[2][1] * dcm[2][1] + dcm[2][2] * dcm[2][2]));
	angle[2] = atan2(dcm[1][0], dcm[0][0]);
	return angle;
}
Vector QuaternionToEulerAngle(const Vector & p)
{
	Matrix temp = QuaternionToDCM(p);
	return DCMToEulerAngle(temp);
}
Matrix generateAntisymmetricMatrix(const Vector & obj)
{
	Matrix m = Matrix(3, 3);
	m[0][0] = 0;
	m[0][1] = -obj[2];
	m[0][2] = obj[1];
	m[1][0] = obj[2];
	m[1][1] = 0;
	m[1][2] = -obj[0];
	m[2][0] = -obj[1];
	m[2][1] = obj[0];
	m[2][2] = 0;
	return m;
}
double getG(double B, double H)
{
	static double grav[] = { 9.7803267715, 0.0052790414, 0.0000232718, -0.000003087691089, 0.000000004397731, 0.000000000000721 };
	double sinb2 = sin(B) * sin(B);
	double sinb4 = sinb2 * sinb2;
	return grav[0] * (1.0 + grav[1] * sinb2 + grav[2] * sinb4) + (grav[3] + grav[4] * sinb2)*H + grav[5] * H*H;
}