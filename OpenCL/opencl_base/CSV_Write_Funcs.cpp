#include "CSV_Write_Funcs.h"

CSV_Writer::CSV_Writer(const std::string &fName) {
	this->fName = fName;
	count = 0;

}

CSV_Writer::~CSV_Writer() {

}

void CSV_Writer::initFile() {
	//This is a function that creates a blank csv file, if the file did not exist before, 
	//or it clears the contents of the file.
	//Either way, this function will also add a header for this csv file.

	std::ofstream ofs;
	ofs.open(this->fName, std::ofstream::out | std::ofstream::trunc);//Clear or create a file.

	ofs << "Amplitude,Frequency" << std::endl; //Add the header to the file.

	ofs.close();
}

void CSV_Writer::writeData(std::vector<double> outData) {
	//This is a function that rights data to a particular file given by fName.
	//This function also adds commas to the output file.
	
	std::ofstream outfile;

	outfile.open(this->fName, std::ios_base::app); //Open the file in append mode.

	//Add the data to the file.
	for (int i = 0; i < outData.size(); i++) {
		outfile << outData.at(i) << std::endl;
	}

	this->count++;
	outfile.close(); //Never forget to close the file :)
}

void CSV_Writer::writeDataByReference(std::vector<double>& outData)
{
	//This is a function that rights data to a particular file given by fName.
	//This function also adds commas to the output file.

	std::ofstream outfile;

	outfile.open(this->fName, std::ios_base::app); //Open the file in append mode.

	//Add the data to the file.
	for (int i = 0; i < outData.size(); i++)
	{
		outfile << outData.at(i) << std::endl;
	}

	this->count++;
	outfile.close(); //Never forget to close the file :)
}

void CSV_Writer::writeNormF(const char* firstNonZTime, int chunksize, int tapsize) {
	//This is a function that adds the normalized frequency values to the file.
	//This function is passed a file pointer that points to the 3rd row in the table,
	//Which is the place where the first non-zero time value occurs and is also
	//effectively the step size of that was used by the MATLAB function to generate the signals.

	std::string store; //String that will hold the first nonzero time value from the file.

	while ((*firstNonZTime) != COMMA) {
		store.push_back((*firstNonZTime));
		firstNonZTime++;
	}

	double nonZero = std::stod(store);

	double samplingF = (1 / nonZero);

	double numOfBins = double(this->count*(chunksize - tapsize));

	std::ifstream inFile;
	std::ofstream outFile;

	inFile.open(fName);
	outFile.open("convolutionFinal.csv", std::ofstream::out | std::ofstream::trunc);

	//Check both files are opened.
	if (!(inFile.is_open()) && (!(outFile.is_open()))) {
		std::cout << "Failed open either the input or the output file!" << std::endl;

		return;
	}

	std::string inString;
	double count = 0.0;
	double cnt_div_numBins = 0.0;
	double normF = 0.0;

	while (std::getline(inFile, inString)) {
		cnt_div_numBins = count / numOfBins;
		normF = cnt_div_numBins * samplingF;

		std::string outString = std::to_string(normF) + "," + inString;

		outFile << outString << std::endl;

		count++;
	
	}
}