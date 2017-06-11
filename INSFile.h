#pragma once
#pragma warning(disable : 4996)

#include <stdio.h>
#include "Enums.h"
#include "Structs.h"
#include "CPTBin.h"
#include "CPTTxt.h"
#include "MTITxt.h"
#include "SBGTxt.h"
#define read_single_line fgets(temp,210,insFile->fp)
class INSFile {
public:
	bool available() {
		if (fp == NULL)return false;
		else {
			return !feof(fp);
		}
	}
	void close() {
		fclose(fp);
		fp = NULL;
	}
	static INSFile * open(const char * file, FileFormat format) {
		INSFile * insFile = new INSFile();
		char temp[210];
		switch (format)
		{
		case CPTbin:			
			insFile->fp = fopen(file, "rb");
			break;
		case MTIbin:
			
			insFile->fp = fopen(file, "rb");
			
			break;
		case CPTtxt:
			insFile->fp = fopen(file, "r");
			break;
		case MTItxt:
			insFile->fp = fopen(file, "r");
			for (int i = 0;i < 5;i++)read_single_line;
			break;
		case SBGtxt:
			insFile->fp = fopen(file, "r");
			for (int i = 0;i < 2;i++)read_single_line;
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