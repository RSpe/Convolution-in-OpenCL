#pragma once
#include <iostream>
#include <utility>

namespace Utils
{
	template <typename T>
	static void print_array(int n, T* _array)
	{
		/*
		pretty print an array
		*/
		std::cout << "[";
		for (auto i = 0; i < n; ++i)
		{
			std::cout << _array[i];
			if (i != n - 1)
			{
				std::cout << ",";
			}
		}
		std::cout << "]" << std::endl;
	}

	template <typename T>
	static void print_complex_array(int n, T** _array)
	{
		/*
			pretty print a complex array
		*/
		std::cout << "[";
		for (auto i = 0; i < n; ++i)
		{
			std::cout << "(" << _array[i][0] << "," << _array[i][1] << "i" << ")";
			if (i != n - 1)
			{
				std::cout << ",";
			}
		}
		std::cout << "]" << std::endl;
	}

	static size_t get_file_length(std::string filepath)
	{
		std::ifstream input_file;
		input_file.open(filepath, std::ios::in);
		input_file.unsetf(std::ios_base::skipws);
		static size_t line_count = std::count(std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>(), '\n');
		return line_count;
	}

	static size_t get_file_size(std::ifstream& file)
	{
		file.seekg(0, file.end);
		size_t length = file.tellg();
		file.seekg(0, file.beg);
		return length;
	}

	static void read_kernel_code(std::ifstream& kernel_file,
										const char* filepath,
										char*& buffer, size_t& size)
	{
		kernel_file.open(filepath, std::ios::in);
		size = get_file_size(kernel_file);
		buffer = new char[size]();
		if (kernel_file.is_open())
		{
			std::cout << "Reading file " << filepath << "\n";
			kernel_file.read(buffer, size);
			kernel_file.close();
			std::cout << "Finished with file " << filepath << "\n";
		}
		else
		{
			std::cerr << "File read failed" << filepath << ". Kernel_file.is_open: "
				<< kernel_file.is_open() << std::endl;
			kernel_file.close();
			exit(-1);
		}
	}

	template <typename T>
	static void print_vector(T vector)
	{
		std::cout << "[";
		for (auto i = 0; i < vector.size(); ++i)
		{
			std::cout << vector[i];
			if (i != vector.size() - 1)
			{
				std::cout << ",";
			}
		}
		std::cout << "]" << std::endl;
	}

	template <typename T>
	static void print_complex_vector(T vector)
	{
		std::cout << "[";
		for (auto i = 0; i < vector.size(); ++i)
		{
			std::cout << vector[i];
			if (i != vector.size() - 1)
			{
				std::cout << ",";
			}
		}
		std::cout << "]" << std::endl;
	}

	/*
	T vector
	U array
	both should be same length so just take vectors size
	*/
	template <typename T, typename U>
	void vector_to_array(T& vector, U& _array)
		/*both should be same length so just take vectors size*/
	{
		for (auto i = 0; i < vector.size(); ++i)
		{
			_array[i] = vector[i];
		}
	}

	/*
	T vector
	U array
	both should be same length so just take vectors size
	*/
	template <typename T, typename U>
	void array_to_vector(T& vector, U& _array)
	{
		for (auto i = 0; i < vector.size(); ++i)
		{
			vector[i] = _array[i];
		}
	}


	/*
	copy a into b
	*/
	template <typename T, typename U>
	void copy_vector(T& a, U& b)
	{
		for (auto i = 0; i < a.size(); ++i)
		{
			b[i] = a[i];
		}
	}
};