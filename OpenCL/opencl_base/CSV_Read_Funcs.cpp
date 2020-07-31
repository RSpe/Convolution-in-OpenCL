#include "CSV_Read_Funcs.h"

void FileRead::getFirstChunk(const char*& memPtr, std::vector<double>& fir,
							 std::vector<double>& signal, size_t CHUNK_SIZE, size_t TAP_SIZE)
{

	/*
		This is a function that instantiates creates a memory mapped file, populates a vectors with the first chunk of
		data and returns a pointer to the memory mapped file.
		This function should only be called to get the first chunk of data.
		I.e. IT SHOULD ONLY EVER BE CALLED ONCE EVER!!!

	*/

	memPtr += FIRSTLINE; //Skip the header of the file

	for (int i = 0; i < (CHUNK_SIZE - TAP_SIZE); i++)
	{
		fillInitData(memPtr, fir, signal, i, CHUNK_SIZE, TAP_SIZE);
	}

	/*Use for testing.

	for (int i = 0; i < (CHUNK_SIZE - TAP_SIZE + 1); i++) {
		std::cout << "fir = " << fir.at(i) << "; signal = " << signal.at(i) << std::endl;

	}
	*/
}

void FileRead::zeroPadding(std::vector<double>& signal, std::vector<double>& signal_ZP,
						   std::vector<double>& hold, size_t CHUNK_SIZE, size_t TAP_SIZE)
{
	//This is a function that takes the signal data that we get from our csv file and 
	//sends the first (CHUNK_SIZE - TAP_SIZE) amount of elements to a new vector.
	//The data will be placed in the new vector at an offset of TAP_SIZE.
	//Then the last TAP_SIZE elements in the signal data will be sent to another
	//vector that will be used to pad data in the next iteration.

	//Filling the vector to be sent to the GPU.
	for (int i = 0; i < (CHUNK_SIZE - TAP_SIZE); i++)
	{
		signal_ZP.at(i + TAP_SIZE) = signal.at(i);

	}

	saveVals(signal, hold, CHUNK_SIZE, TAP_SIZE); //Save the last TAP_SIZE amount of data points for the next iteration.

}

void FileRead::saveVals(std::vector<double>& signal, std::vector<double>& hold,
						size_t CHUNK_SIZE, size_t TAP_SIZE)
{
	//This for loop holds the last TAP_SIZE amount of element that will be used in the next iteration.
	for (int i = 0; i < TAP_SIZE; i++)
	{
		hold.at(i) = signal.at(i + (CHUNK_SIZE - TAP_SIZE - TAP_SIZE));
	}

}

void FileRead::skipCol(const char*& memPtr)
{
	//This is a function that skips a column of data.
	while ((*memPtr) != COMMA)
	{
		memPtr++;
	}

	memPtr++; //Move the pointer past the comma and on to the next column.

}

void FileRead::fillInitData(const char*& memPtr, std::vector<double>& fir,
							std::vector<double>& signal, int currentPos,
							size_t CHUNK_SIZE, size_t TAP_SIZE)
{
	/*
	This function performs 3 tasks:
		1) getLine()
		2) Sends the values that are stored in the line into vectors that hold the data.
		3) Update the pointer so that it points to the start of the next line.

	This function is only called once to get both the FIR coefficients and the first chunk of data.
	*/

	std::string test;
	std::stringstream ss;

	skipCol(memPtr); //skip the time column

	//Relevant data is only available before the carriage return character. This is the getLine() functionality.
	while ((*memPtr) != CARRIAGE_RETURN)
	{

		if ((*memPtr) == COMMA)
		{
			test.push_back(SPACE);
		}
		else
		{
			test.push_back((*memPtr));
		}

		memPtr++;

	}

	//Sending data to vectors.
	ss << test;
	ss >> fir.at(currentPos) >> signal.at(currentPos);

	//Use this for testing.

	/*
		std::cout << test << std::endl;

		std::cout << "fir = " << fir.at(currentPos) << std::endl;
		std::cout << "signal = " << signal.at(currentPos) << std::endl;

	*/
	memPtr += 2; //Update the file pointer so that it jumps onto the next line.

}

void FileRead::grabFirCoef(std::vector<double>& fir, std::vector<double>& tap_coef,
						   size_t CHUNK_SIZE, size_t TAP_SIZE)
{
	/*
		This is a helper function that grabs all of the FIR filter coefficients that were stored in our file.
	*/

	for (int i = 0; i < TAP_SIZE; i++)
	{
		tap_coef.at(i) = fir.at(i);
	}
}

int FileRead::getNextChunk(const char*& memPtr, std::vector<double>& signal,
						   const char* lastByte, size_t CHUNK_SIZE, size_t TAP_SIZE)
{
	//This is a function that gets the next (CHUNK_SIZE - TAP_SIZE) amount of values from the file.
	//If we have reached the end of the file while getting a chunk of data from the file 
	//then we return "FALSE" preventing the program from getting another chunk afterwards.
	//Otherwise, we will return "TRUE" allowing the program to continue chunking.

	for (int i = 0; i < (CHUNK_SIZE - TAP_SIZE); i++)
	{
		//If we have reached the end of the file then we break out of the loop.
		if (memPtr == lastByte)
		{
			fillZeros(signal, i, CHUNK_SIZE, TAP_SIZE);
		}
		else
		{
			getSigFromFile(memPtr, signal, i, CHUNK_SIZE, TAP_SIZE);
		}
	}

	if (memPtr == lastByte)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}

void FileRead::getSigFromFile(const char*& memPtr, std::vector<double>& signal,
							  int currentPos, size_t CHUNK_SIZE, size_t TAP_SIZE)
{
	std::string test;
	std::stringstream ss;

	skipCol(memPtr); //skip the time column
	skipCol(memPtr); //skip the FIR column

	//Relevant data is only available before the carriage return character. This is the getLine() functionality.
	while ((*memPtr) != CARRIAGE_RETURN)
	{

		if ((*memPtr) == COMMA)
		{
			test.push_back(SPACE);
		}
		else
		{
			test.push_back((*memPtr));
		}

		memPtr++;

	}

	//Sending data to vectors.
	ss << test;
	ss >> signal.at(currentPos);

	memPtr += 2; //Update the file pointer so that it jumps onto the next line.

}

void FileRead::padWithHold(std::vector<double>& signal, std::vector<double>& signalPadded,
						   std::vector<double>& hold, size_t CHUNK_SIZE, size_t TAP_SIZE)
{
	//This is a function that pads the start of the data with the values that we held onto 
	//from the previos iteration.

	for (int i = 0; i < CHUNK_SIZE; i++)
	{
		if (i < TAP_SIZE)
		{
			signalPadded.at(i) = hold.at(i); //Padd with old data
		}
		else
		{
			signalPadded.at(i) = signal.at(i - TAP_SIZE); //Finish chunk with new data.
		}
	}

	saveVals(signal, hold, CHUNK_SIZE, TAP_SIZE); //Save some data for the next iteration.
}

void rinseVectors(std::vector<double>& inVec)
{
	//This is a function to set the values inside of a vector back down to zero.
	std::fill(inVec.begin(), inVec.end(), 0.0);

}

void FileRead::fillZeros(std::vector<double>& signal, int currentPos, size_t CHUNK_SIZE, size_t TAP_SIZE)
{
	//This is a function to fill up a chunk with zeros if we have reached the 
	//end of the file.
	signal.at(currentPos) = 0.0;
}

void FileRead::skipLine(const char*& memPtr, size_t CHUNK_SIZE, size_t TAP_SIZE)
{
	while ((*memPtr) != NEWLINE)
	{
		memPtr++;
	}

	memPtr++; //Make the pointer jump to the next line.
}