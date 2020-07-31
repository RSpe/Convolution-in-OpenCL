close all;
clear;

%This is a function to create a graph the results of our parallel
%convolution code using openCL in C++. We also print out the input 
%to the code.

%Code below opens the csv, drops the commas and skips the header
%for the input we used for our openCL code.
inputData = importdata('signal_and_filter.csv',',',1);
time = inputData.data(:,1);
inputSignal = inputData.data(:,3);

period = time(2);
scale = 2000*period;

%Code below opens the csv, drops the commas and skips the header
%for the results of our openCL code.
openCLdata = importdata('convolutionFinal.csv', ',', 1);
normF = openCLdata.data(:,1);
amplitude = openCLdata.data(:,2);

figure(1)
subplot(2,1,1);
plot(time, inputSignal);
title('Input to openCL C++ code')
xlabel('Time (s)')
ylabel('Amplitude')
xlim([0 scale])

subplot(2,1,2);
semilogx(normF, amplitude);
title('Output of convolution(Overlap-Save) using openCL in C++')
xlabel('Normalized Frequency (HZ), Log Scale')
ylabel('Amplitude')

