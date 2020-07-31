# Topic
## 1) Convolution in OpenCL
Finite Impulse Response (FIR) filters, or convolution, is one of the most essential techniques in signal processing <https://en.wikipedia.org/wiki/Finite_impulse_response>. For example, they are heavily used in the data processing in radio astronomy, e.g. in the Square Kilomtre Array (SKA) project <https://www.skatelescope.org>. Relatively short filters for not too long 1D input data can be implemented with a quite straight forward approach, consisting of multiplications and additions. For very large inputs or large filters the data needs to be divided into chunks, filtered, and then the results need to be combined in simple, but not trivial way. Also, for large filters there can be a very strong computational advantage to transfer the input data and filter into the frequency domain (using an FFT), then filter, and then transfer back into time domain. 
This project implements a 1D-FIR filter with N coefficients (N being a parameter) to filter input data of length M (M being a parameter), where each value (input data and filter coefficient) is a single precision floating point number. Chunking input data and Overlap save is used to process the large input stream.


Setup:

# Windows: NVIDIA GPU
1. download Visual studio 2019
2. Download and install https://developer.nvidia.com/cuda-toolkit custom install and only install things you want (probably don't want drivers etc). Leave install path as C:\Program Files\NVIDIA GPU Computing Toolkit, otherwise you will have to change visual studio configs.
3. Open .sln in opencl_base and run it. This is just some random C sample code from the internet, run it to check if your env works.

# Windows: OTHER 
* AMD: https://github.com/GPUOpen-LibrariesAndSDKs/OCL-SDK/releases
point visual studio to the lib and include. linker/additional lib directories,  linker/dependencies and C/C++/additional include dirs. See if you can run the sample code in opencl_base.
* INTEL: https://software.intel.com/content/www/us/en/develop/tools/opencl-sdk.html

# FFTW windows:
FFTW install/setup.

1. Download fftw DLL.

2. run lib /def:libfftw3-3.def or lib /machine:x64 /def:libfftw3-3.def on powershell or cmd 
	(needs to be the one you laucnh from VS)
	
3. Make a folder on your pc and place the corresponding .lib, .exp and .h in it.
	e.g. ~/FFTW/lib/libfftw3-3.lib, ~/FFTW/lib/libfftw3-3.exp and ~/FFTW/include/fftw3.h
	
4. Go to project properties and make sure the configuration is ALL configurations.

5. From C/C++ General - > Additional Include Directories. Enter the folder you just made with the .h

6. From Linker General - > Additional Library Directories. Enter the folder you just made with the .lib

7. From Linker Input - > Additional Dependencies. libfftw3-3.lib or your version.

8. Copy paste DLL into same dir as source (libfftw3-3.dll for doubles)

9. Make sure the run config is x64 NOT x86 or things WILL NOT work.

# Vex CL
https://github.com/ddemidov/vexcl
Header only but depends on boost, and the parts of boost that need to be compiled.

# Boost
https://www.boost.org/doc/libs/1_73_0/more/getting_started/windows.html
Will need to build then link to compiled folders.
