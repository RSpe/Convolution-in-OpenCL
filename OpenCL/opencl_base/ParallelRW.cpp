#include "ParallelRW.h"

int run(std::string input_filepath, std::string output_filepath, const int CHUNK_SIZE, const int TAP_SIZE) {

	//=====================================VARIABLE DECLARATIONS=============================================================//
	const char* filePtr = NULL; //Pointer to the memory mapped file.
	const char* startOfFilePtr = NULL;
	const char* endOfFilePtr = NULL;

	//Vectors that hold single precsion float data
	std::vector<double> fir(CHUNK_SIZE);
	std::vector<double> signal(CHUNK_SIZE);

	//Ensure that the FIR and signal vectors are instantiated with all zeros.
	std::fill(fir.begin(), fir.end(), 0.0);
	std::fill(signal.begin(), signal.end(), 0.0);

	//These are the variables that will hold the actual useful values.
	std::vector<double> tap_coef(TAP_SIZE);
	std::vector<double> signalPadded(CHUNK_SIZE);
	std::vector<double> hold(TAP_SIZE);

	//Ensuring that the initial value of these vectors are all zero.
	std::fill(signalPadded.begin(), signalPadded.end(), 0.0);
	std::fill(hold.begin(), hold.end(), 0.0);

	CSV_Writer *csv_writer = new CSV_Writer("convolution_results.csv");
	csv_writer->initFile(); //Create or clear the output file.

	boost::iostreams::mapped_file_source csv("signal_and_filter.csv"); //Maps the entire file to a virtual memory space.
	filePtr = csv.data();
	startOfFilePtr = filePtr;
	endOfFilePtr = filePtr + csv.size(); //This is the last bit of data.

	//=====================================FILE CHECK=============================================================//
	//Test if the mapping was successful.
	if (csv.is_open()) {
		std::cout << "Successfully opened the CSV!" << std::endl;
	}
	else {
		std::cout << "Failed to open the CSV!" << std::endl;

		return -1;
	}

	//=====================================CHUNK 1=============================================================//
	FileRead::getFirstChunk(filePtr, fir, signal, CHUNK_SIZE, TAP_SIZE);

	FileRead::grabFirCoef(fir, tap_coef, CHUNK_SIZE, TAP_SIZE); //Get the value of the coefficients.

	FileRead::zeroPadding(signal, signalPadded, hold, CHUNK_SIZE, TAP_SIZE); //Apply zero padding to the first set of data.

	for (auto& value : signal)
	{
		std::cout << value  << "\n";
	}
	//=====================================PARALLEL WORK=============================================================//

	//send data to openCL/////////////////////////////////////////////////////////////////////////////////////////////

	int keepChunking = TRUE;
	keepChunking = FileRead::getNextChunk(filePtr, signal, endOfFilePtr, CHUNK_SIZE, TAP_SIZE); //Get the next chunk
	FileRead::padWithHold(signal, signalPadded, hold, CHUNK_SIZE, TAP_SIZE); //Add the previous elements to this chunk.

	//Wait for OpenCL and Read Next Chunk////////////////////////////////////////////////////////////////////////////

	//=====================================WRITING STARTS=============================================================//
	while (keepChunking == TRUE) {

		//send data to opencl/////////////////////////////////////////////////////////////////////////////////////////////

		std::thread writer(&CSV_Writer::writeData, csv_writer, signal); //starting storing results.

		keepChunking = FileRead::getNextChunk(filePtr, signal, endOfFilePtr, CHUNK_SIZE, TAP_SIZE); //Get the next chunk
		FileRead::padWithHold(signal, signalPadded, hold, CHUNK_SIZE, TAP_SIZE);

		writer.join();

		//Wait for OpenCL//////////////////////////////////////////////////////////////////////////////////////////////

	}

	//=====================================Final Memory Write=============================================================//
	std::thread writer(&CSV_Writer::writeData, csv_writer, signal); //starting storing results.
	writer.join();

	//====================================STOP TIMING HERE===============================================================//
	startOfFilePtr += FIRSTLINE;
	FileRead::skipLine(startOfFilePtr, CHUNK_SIZE, TAP_SIZE);
	//csv_writer->writeNormF(startOfFilePtr, CHUNK_SIZE, TAP_SIZE);

	//=====================================RELEASE MEMORY=============================================================//
	csv.close(); //Close the file like a good boy :)
	delete csv_writer;
	std::cout << "Congratulations you have read through the whole file!" << std::endl;

}