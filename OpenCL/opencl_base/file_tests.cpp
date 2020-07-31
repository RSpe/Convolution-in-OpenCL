#include "file_tests.h"

void testVals(std::vector<double> testVec, int length,const std::string hint) {

	//This is a function that I use to output data for checking.

	for (int i = 0; i < length; i++) {
		std::cout << hint << testVec.at(i) << std::endl;
		
	}
}

void testTwo(std::vector<double> test1, const std::string hint1, 
				std::vector<double> test2, const std::string hint2, int length) {

	// This is a function that I use to output two vectors of data, for checking.
	//THE LENGTH OF BOTH VECTORS SHOULD BE THE SAME!

		for (int i = 0; i < length; i++) {
			std::cout << hint1 << test1.at(i) << hint2 << test2.at(i) << std::endl;

		}

}