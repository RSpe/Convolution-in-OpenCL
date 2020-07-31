#pragma once
#include <iostream>
#include <algorithm>
#include <thread>
#include "CSV_Read_Funcs.h"
#include "CSV_Write_Funcs.h"
#include "file_tests.h"

int run(std::string input_filepath, std::string output_filepath, const int CHUNK_SIZE, const int TAP_SIZE);