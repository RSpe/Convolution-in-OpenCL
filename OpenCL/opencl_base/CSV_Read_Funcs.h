#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "boost/iostreams/device/mapped_file.hpp"

namespace FileRead
{
#define FIRSTLINE 17 //the define above has to be 17 because "time,fir,signal" is 15 characters and the last 2 characters are \n\r.

#define NEWLINE '\n'
#define COMMA ','
#define CARRIAGE_RETURN '\r'
#define TERMINATE '\0'
#define SPACE ' '
#define TRUE 1
#define FALSE 0

	void getFirstChunk(const char*& memPtr, std::vector<double>& fir, std::vector<double>& signal, size_t CHUNK_SIZE, size_t TAP_SIZE); //
	void fillInitData(const char*& memPtr, std::vector<double>& fir, std::vector<double>& signal, int currentPos, size_t CHUNK_SIZE, size_t TAP_SIZE); //
	void grabFirCoef(std::vector<double>& fir, std::vector<double>& tap_coef, size_t CHUNK_SIZE, size_t TAP_SIZE); //
	void zeroPadding(std::vector<double>& signal, std::vector<double>& signal_ZP, std::vector<double>& hold, size_t CHUNK_SIZE, size_t TAP_SIZE); //
	void skipCol(const char*& memPtr);
	int getNextChunk(const char*& memPtr, std::vector<double>& signal, const char* lastByte, size_t CHUNK_SIZE, size_t TAP_SIZE);
	void getSigFromFile(const char*& memPtr, std::vector<double>& signal, int currentPos, size_t CHUNK_SIZE, size_t TAP_SIZE);
	void padWithHold(std::vector<double>& signal, std::vector<double>& signalPadded, std::vector<double>& hold, size_t CHUNK_SIZE, size_t TAP_SIZE);
	void rinseVectors(std::vector<double>& inVec, size_t CHUNK_SIZE, size_t TAP_SIZE);
	void saveVals(std::vector<double>& signal, std::vector<double>& hold, size_t CHUNK_SIZE, size_t TAP_SIZE);
	void fillZeros(std::vector<double>& signal, int currentPos, size_t CHUNK_SIZE, size_t TAP_SIZE);
	void skipLine(const char*& memPtr, size_t CHUNK_SIZE, size_t TAP_SIZE);
}