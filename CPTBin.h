#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "Structs.h"
#include "CRC.h"

#define GSF 1.0850694444444444444444444444444e-7
#define ASF 0.00000152587890625


#define uchar unsigned char
#define ushort unsigned short
#define ulong unsigned long
#define uint unsigned int
#define buffer_size 100

const short crc_size = 4;
const uchar head_flag[3] = { 0xAA,0x44,0x13 };//3 Sync bytes
const short id_imu = 0x145;
const short status_fine = 0x77;

struct FileHeader
{
	uchar sync_flag[3];
	uchar message_length;
	ushort message_id;
	ushort week_number;
	ulong milliseconds;
};
struct RAWIMUS {
	ulong week;
	double sec;
	long imu_status;

	long AZ;
	long AY;
	long AX;
	long GZ;
	long GY;
	long GX;
};
struct RAWIMUSMessage {
	FileHeader header;
	RAWIMUS body;
};
ushort get_ushort(void * pointee)
{
	ushort tot = 0;
	memcpy(&tot, pointee, 2);
	return tot;
}
ulong get_ulong(void * pointee)
{
	ulong tot = 0;
	memcpy(&tot, pointee, 4);
	return tot;
}
uchar get_uchar(void * pointee)
{
	uchar tot = 0;
	memcpy(&tot, pointee, 1);
	return tot;
}

void cpt_bin_input_data(FILE * fp, INSData * out)
{
	long origin = ftell(fp);
	void * temp_buffer = alloca(buffer_size);
	bool flag_data_in_position = false;
	int message_length = 0;
	int message_id = 0;
	while (!flag_data_in_position)
	{
		fread(temp_buffer, buffer_size, 1, fp);
		for (int i = 0; i<buffer_size - 3;i++)
		{
			if (memcmp(((uchar*)temp_buffer) + i, head_flag, 3) == 0)
			{
				message_length = get_uchar((uchar*)temp_buffer + i + 3);
				message_id = get_ushort((uchar*)temp_buffer + i + 4);
				if (i == 0)fseek(fp, origin, SEEK_SET);
				else fseek(fp, i - buffer_size, SEEK_CUR);
				flag_data_in_position = true;
				break;
			}
		}
	}
	int frame_size = 12 + message_length;
	void * data = alloca(frame_size);
	void * crc = alloca(crc_size);
	fread(data, frame_size, 1, fp);
	fread(crc, crc_size, 1, fp);
	if (message_id == id_imu)
	{
		int length_2 = get_uchar((uchar*)data + 3);
		RAWIMUSMessage * casted_data = (RAWIMUSMessage*)data;
		if (casted_data->body.imu_status == status_fine)
		{
			out->Time = casted_data->body.sec;
			out->A[0] = casted_data->body.AX * ASF;
			out->A[1] = -casted_data->body.AY * ASF;
			out->A[2] = casted_data->body.AZ * ASF;
			out->G[0] = casted_data->body.GX * GSF;
			out->G[1] = -casted_data->body.GY * GSF;
			out->G[2] = casted_data->body.GZ * GSF;
		}
	}
	ulong crc_file = get_ulong(crc);
	ulong crc_calc = CalculateBlockCRC32(frame_size, (uchar*)data);
	if (crc_file != crc_calc)
	{
		throw "fatal error 0005 : crc check failed in frame in cpt bin file.";
	}
}