%This matlab script will generate a file and images that can be used to
%test our C++ code when the size of the input is 10,000.

clc 
clear;
close all

sampleSize = 1000000;
samplePow2 = 2^nextpow2(sampleSize);

Fs = 100000; %Sampling frequency (Hz).
Ts = 1/Fs; %Sampling period (S).
t = linspace(0, (1-Ts), samplePow2); %Duration of the signal(/s).

%Scale factor for different frequencies.
%Note: keep the scales below 0.1.
k1 = 0.025;
k2 = 0.1; 
k3 = 0.056;
k4 = 0.07; 

%Different frequencies in our signal.
f1 = Fs*k1;
f2 = Fs*k2;
f3 = Fs*k3;
f4 = Fs*k4;

max_f = max([f1 f2 f3 f4]); %Finds the largest value out of the 4 frequencies.
scaler = (2*pi)/max_f;

%Amplitudes for each signal.
A = 10;
B = 2;
C = 5;
D = 1;

%Individual signals.
y1 = A*(sin(2*pi*f1*t));
y2 = B*(sin(2*pi*f2*t));
y3 = C*(sin(2*pi*f3*t));
y4 = D*(sin(2*pi*f4*t));

%Combined signal.
yt = y1+y2+y3+y4;

%To make that our fft ends up looking correctly we need to give the fft a
%length that is = 2^x; where x = is any integer.
nfft = length(yt);
nfft2 = 2^nextpow2(nfft);

%Perform FFT
y_freq = fft(yt,nfft2); %FFT gives magnitude and phase.
y_freq = y_freq(1:nfft2/2); %Only want to see the +ve frequencies.

norm_x = Fs.*(0:(nfft2/2-1))/nfft2; %Normalize the x axis (frequency).   

cut_off_freq = f1/(Fs/2); %Have to normalize the cutoff frequency with Nyquist.
order = 512; %64 tap filter.

lp_fil = fir1(order,cut_off_freq); % Get the low pass filter output.

lp_fil_freq = fft(lp_fil, nfft2); %Freq. convolution.
lp_fil_freq = lp_fil_freq(1:(nfft2/2));

freq_conv = lp_fil_freq.*y_freq;

temp_arr = zeros(1, length(t));
temp_arr(1:length(lp_fil)) = lp_fil;

dataTable = table();
dataTable.time = transpose(t);
dataTable.fir = transpose(temp_arr);
dataTable.signal = transpose(yt);

writetable(dataTable, 'results_2x3.csv')
