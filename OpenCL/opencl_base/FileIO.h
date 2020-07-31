#pragma once
#include <vector>
#include <string>
#include "CSV_Read_Funcs.h"
#include "CSV_Write_Funcs.h"


namespace FileIO
{
	struct FileContext
	{
		// Read head
		const char* filePtr { nullptr };
		const char* startOfFilePtr { nullptr };
		const char* endOfFilePtr { nullptr };

		const char* input_filepath { nullptr };
		const char* output_filepath { nullptr };

		size_t CHUNK_SIZE {};
		size_t TAP_SIZE {};

		CSV_Writer* csv_writer { nullptr };
		boost::iostreams::mapped_file_source csv {};

		std::vector<double> fir {};
		std::vector<double> signal {};
		std::vector<double> signalPadded {};
		std::vector<double> out_signal {};

		std::vector<double> tap_coef {};
		std::vector<double> hold {};
	};


	/*
	Have to set vector sizes due to inital use of container needing to access by index.
	This function sets all the vectors associated with reading and wrting the file
	to correct sizes.
	*/
	void set_vector_sizes(FileContext& data)
	{
		data.fir.resize(data.CHUNK_SIZE, 0);
		data.signal.resize(data.CHUNK_SIZE, 0);
		data.out_signal.resize(data.CHUNK_SIZE, 0);
		data.signalPadded.resize(data.CHUNK_SIZE, 0);
		data.tap_coef.resize(data.TAP_SIZE, 0);
		data.hold.resize(data.TAP_SIZE, 0);
	}

	/*
	Create the mapped file.
	Set the read head and other pointers to values attained from
	mapped file.
	*/
	void map_file(FileContext& data)
	{
		data.csv = boost::iostreams::mapped_file_source(data.input_filepath);
		data.filePtr = data.csv.data();
		data.startOfFilePtr = data.filePtr;
		data.endOfFilePtr = data.filePtr + data.csv.size(); //This is the last bit of data.
	}

	/*
	Setup the write object
	*/
	void set_csv_writer(FileContext& data)
	{
		data.csv_writer = new CSV_Writer(data.output_filepath);
		data.csv_writer->initFile(); //Create or clear the output file.
	}

	/*
	Check if the file mapped correctly
	*/
	bool file_context_is_mapped(FileContext& data)
	{
		if (data.csv.is_open())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/*
	Perform the first read on the mapped file
	Padd first block too
	*/
	void start_read(FileContext& data)
	{
		FileRead::getFirstChunk(data.filePtr, data.fir, data.signal, data.CHUNK_SIZE, data.TAP_SIZE);
		//Get the value of the coefficients.
		FileRead::grabFirCoef(data.fir, data.tap_coef, data.CHUNK_SIZE, data.TAP_SIZE);
		//Apply zero padding to the first set of data.
		FileRead::zeroPadding(data.signal, data.signalPadded, data.hold, data.CHUNK_SIZE, data.TAP_SIZE);
	}

	/*
	Perform the first read on the mapped file
	*/
	void start_read_no_pad(FileContext& data)
	{
		FileRead::getFirstChunk(data.filePtr, data.fir, data.signal, data.CHUNK_SIZE, data.TAP_SIZE);
		//Get the value of the coefficients.
		FileRead::grabFirCoef(data.fir, data.tap_coef, data.CHUNK_SIZE, data.TAP_SIZE);
	}

	/*
	Get the next chunk of the file using padding.
	returns a bool for looping.
	*/
	bool get_next_padded_chunk(FileContext& data)
	{
		bool next_chunk { true };
		next_chunk = FileRead::getNextChunk(data.filePtr, data.signal, data.endOfFilePtr, data.CHUNK_SIZE, data.TAP_SIZE);
		//Add the previous elements to this chunk.
		FileRead::padWithHold(data.signal, data.signalPadded, data.hold, data.CHUNK_SIZE, data.TAP_SIZE);
		return next_chunk;
	}

	/*
	Get the next chunk of the file.
	returns a bool for looping.
	*/
	bool get_next_chunk(FileContext& data)
	{
		bool next_chunk { true };
		next_chunk = FileRead::getNextChunk(data.filePtr, data.signal, data.endOfFilePtr, data.CHUNK_SIZE, 0);
		return next_chunk;
	}

	/*
	Write option that passes by reference.
	*/
	void write_chunk(FileContext& data)
	{
		data.csv_writer->writeDataByReference(data.out_signal);
	}
	
	/*
	Write option that passes by value,
	with very large chunk sizes might perform poorly.
	*/
	void write_chunk_thread_safe(FileContext& data)
	{
		data.csv_writer->writeData(data.out_signal);
	}
}