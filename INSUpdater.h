#pragma once
#include "INSFile.h"
#include "ResultFile.h"
#include "DetailedFunctions.h"
#include "MatCpp.h"
using namespace CppToolBox;
double init_motion[]{ 30.53103519 * PI / 180.0,114.35684659 * PI / 180.0,42.216 };
double init_velocity[]{ 0,0,0 };
double init_position[]{ 0,0,0 };

class INSUpdater {
public:
	INSUpdater(INSFile * ifile, ResultFile * ofile)
	{
		insFile = ifile;
		resultFile = ofile;
		is_first = true;

		motion_previous = Vector(init_motion, 3);
		motion_present = Vector(3);
		velocity_previous = Vector(init_velocity, 3);
		velocity_present = Vector(3);
		position_previous = Vector(init_position, 3);
		position_present = Vector(3);

		result = new INSResult();

		grab_one_data();
		reset();
	}
	bool available()
	{
		return insFile->available();
	}
	void dispose()
	{
		insFile->close();
		resultFile->close();
	}
	void update()
	{
		grab_one_data();
		motion_update();
		velocity_update();
		position_update();
		reset();

		resultFile->append(result);
	}
private:
	void grab_one_data()
	{
		INSData temp;
		insFile->grab(&temp);
		memcpy(obsg_present, temp.G, sizeof(double) * 3);
		memcpy(obsa_present, temp.A, sizeof(double) * 3);
		obstime_present = temp.Time;
	}

	INSFile * insFile;
	ResultFile * resultFile;

	//全局生命量
	bool is_first;
	INSResult * result;

	//历元间暂存量
	Vector motion_previous;
	Vector velocity_previous;
	Vector position_previous;

	double obstime_previous;
	double obsg_previous[3];
	double obsa_previous[3];

	Vector motion_present;
	Vector velocity_present;
	Vector position_present;

	double obstime_present;
	double obsg_present[3];
	double obsa_present[3];


	//历元内暂存量
	Vector qk_1_out;
	Matrix Cbk_1nk_1;


	void motion_update()
	{
		if (is_first)
		{
			Vector qk_1_u = EulerAngleToQuaternion(motion_previous.get_array());
			Vector qk_1 = QuaternionNorm(qk_1_u);
			qk_1_out = qk_1;
			is_first = false;
		}

		Cbk_1nk_1 = QuaternionToDCM(qk_1_out);

		Vector dthetab = Vector(obsg_previous, 3);
		Vector dtheta = Vector(obsg_present, 3);
		Vector faik = dthetab.cross(dtheta) / 12.0 + dtheta;
		double faik_mod = faik.mod();
		Vector qbkbk_1 = getQ(cos(0.5*faik_mod), 0.5*sin(0.5*faik_mod) / (0.5*faik_mod), faik);
		Vector omegaien = Vector(3);
		omegaien[0] = cos(position_previous[0]) * omega;
		omegaien[1] = 0;
		omegaien[2] = -sin(position_previous[0]) * omega;
		double Rn = getRn(position_previous[0]);
		double Rm = getRm(position_previous[0]);
		Vector omegaenn = Vector(3);
		omegaenn[0] = velocity_previous[1] / (Rn + position_previous[2]);
		omegaenn[1] = -velocity_previous[0] / (Rm + position_previous[2]);
		omegaenn[2] = -velocity_previous[1] * tan(position_previous[0]) / (Rn + position_previous[2]);
		Vector ritak = (omegaenn + omegaien) * (obstime_present - obstime_previous);
		double ritak_mod = ritak.mod();
		Vector qnk_1nk = getQ(cos(0.5*ritak_mod), -0.5*sin(0.5*ritak_mod) / (0.5*ritak_mod), ritak);
		Vector qk_u = QuaternionMultiplyAPB(qnk_1nk, qk_1_out, qbkbk_1);
		Vector qk = QuaternionNorm(qk_u);
		Vector angle = QuaternionToEulerAngle(qk);

		motion_present[0] = angle[0];
		motion_present[1] = angle[1];
		motion_present[2] = angle[2];
		qk_1_out = qk;
	}

	void velocity_update()
	{
		Vector dvb = Vector(obsa_previous, 3);
		Vector dv = Vector(obsa_present, 3);
		Vector dthetab = Vector(obsg_previous, 3);
		Vector dtheta = Vector(obsg_present, 3);
		Vector temp3 = dthetab.cross(dv) / 12.0 + dvb.cross(dtheta) / 12.0;
		Vector temp32 = dtheta.cross(dv) / 2 + temp3;
		Vector dvfkbk_1 = dv + temp32;

		Vector omegaien = Vector(3);
		omegaien[0] = cos(position_previous[0]) * omega;
		omegaien[1] = 0;
		omegaien[2] = -sin(position_present[0]) * omega;
		double Rn = getRn(position_previous[0]);
		double Rm = getRm(position_previous[0]);
		Vector omegaenn = Vector(3);
		omegaenn[0] = velocity_previous[1] / (Rn + position_previous[2]);
		omegaenn[1] = -velocity_previous[0] / (Rm + position_previous[2]);
		omegaenn[2] = -velocity_previous[1] * tan(position_previous[0]) / (Rn + position_previous[2]);
		Vector ritak_1k = (omegaenn + omegaien) * (obstime_present - obstime_previous);
		Matrix ritaAM = generateAntisymmetricMatrix(ritak_1k);
		Matrix I3 = Matrix::eye(3);
		Matrix dvfkn = (I3 - ritaAM*0.5) * Cbk_1nk_1 * dvfkbk_1;
		Vector gln = Vector(3);
		gln[0] = 0;
		gln[1] = 0;
		gln[2] = getG(position_previous[0], position_previous[2]);
		Vector temp3_2 = (omegaien * 2 + omegaenn);
		Vector v2 = Vector(velocity_previous);
		Vector temp32_2 = temp3_2.cross(v2);
		Vector dvgkn = (gln - temp32_2) * (obstime_present - obstime_previous);
		velocity_previous = v2 + dvfkn + dvgkn;
	}

	void position_update()
	{
		Vector v2 = velocity_previous;
		Vector v = velocity_present;
		double diff_time = obstime_present - obstime_previous;
		double H0 = position_previous[2] - 0.5*(v[2] + v[2])*diff_time;
		double B0 = position_previous[0] + 0.5*(v[0] + v2[0]) / (getRm(position_previous[0]) + 0.5*(H0 + position_previous[2]))*diff_time;
		double L0 = position_previous[1] + 0.5*(v[1] + v2[1]) / ((getRn(0.5*(B0 + position_previous[0])) + 0.5*(H0 + position_previous[2]))*cos(0.5*(B0 + position_previous[0])))*diff_time;

		position_present[0] = B0;
		position_present[1] = L0;
		position_present[2] = H0;
	}

	void reset()
	{
		//封装result
		memcpy(result->Motion, motion_present.get_array(), sizeof(double) * 3);
		memcpy(result->Velocity, velocity_present.get_array(), sizeof(double) * 3);
		memcpy(result->Position, position_present.get_array(), sizeof(double) * 3);
		result->Time = obstime_present;

		obstime_previous = obstime_present;
		motion_previous = motion_present;
		velocity_previous = velocity_present;
		position_previous = position_present;
		for (int i = 0;i < 3;i++)
		{
			obsg_previous[i] = obsg_present[i];
			obsa_previous[i] = obsa_present[i];
		}
	}
};