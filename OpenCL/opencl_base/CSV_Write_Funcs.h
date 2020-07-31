#pragma once
#include <iostream>
#include <fstream>  
#include <string>
#include <vector>

#define COMMA ','

class CSV_Writer {

public: 

	CSV_Writer(const std::string &fName);
	~CSV_Writer();

	void initFile();
	void writeData(std::vector<double> outData);
	void writeDataByReference(std::vector<double>& outData);
	void writeNormF(const char* firstNonZTime, int chunksize, int tapsize);

private:
	int count;
	std::string fName;

};