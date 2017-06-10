#pragma once
#include <stdio.h>
#include "Enums.h"
#include "Structs.h"
#include "CPTBin.h"
#include "CPTTxt.h"
#include "MTITxt.h"
#include "SBGTxt.h"
class INSFile {
public:
	bool available() {
		return fp != NULL;
	}
	void close() {
		fclose(fp);
	}
	static INSFile * open(const char * file, FileFormat format) {
		INSFile * insFile = new INSFile();
		switch (format)
		{
		case CPTbin:			
			insFile->fp = fopen(file, "rb");
			break;
		case MTIbin:
			char temp[200];
			insFile->fp = fopen(file, "rb");
			for (int i = 0;i < 5;i++)fscanf(insFile->fp, "%s", temp);
			break;
		case CPTtxt:
			insFile->fp = fopen(file, "r");
			break;
		case MTItxt:
			insFile->fp = fopen(file, "r");
			break;
		case SBGtxt:
			char temp[200];
			insFile->fp = fopen(file, "r");
			for (int i = 0;i < 2;i++)fscanf(insFile->fp, "%s", temp);
			break;
		}
		if (insFile->fp == NULL)throw "fatal error 000";
		insFile->fileFormat = format;
		insFile->fileMode = FileOpen;
		return insFile;
	}
	void grab(INSData * data) {
		if (fileMode != FileOpen)throw "fatal error 003";
		else {
			switch (fileFormat)
			{
			case CPTbin:
				cpt_bin_input_data(fp, data);
				break;
			case CPTtxt:
				cpt_txt_input_data(fp, data);
				break;
			case MTIbin:
				throw "unfinished function.";
				break;
			case MTItxt:
				mti_txt_input_data(fp, data);
				break;
			case SBGtxt:
				sbg_txt_input_data(fp, data);
				break;
			default:
				throw "error 004";
			}
		}
	}
private:
	INSFile() : fp(NULL){}
	FILE * fp;
	FileMode fileMode;
	FileFormat fileFormat;
};