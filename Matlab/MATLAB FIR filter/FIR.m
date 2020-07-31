clc 
clear;
close all

Fs = 100000; %Sampling frequency (Hz).
Ts = 1/Fs; %Sampling period (S).
t = 0:Ts:1-Ts; %Duration of the signal(/s).

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

%========================Plotting the component waves.=====================
figure(1);
subplot(2,2,1)
plot(t,y1);
title_str = sprintf('Frequency at %d Hz', f1);
ylabel('amplitude')
xlabel('Seconds(s)')
title(title_str)
xlim([0 (4/f1)])

subplot(2,2,2)
plot(t,y2);
title_str = sprintf('Frequency at %d Hz', f2);
ylabel('amplitude')
xlabel('Seconds(s)')
title(title_str)
xlim([0 (4/f2)])

subplot(2,2,3)
plot(t,y3);
title_str = sprintf('Frequency at %d Hz', f3);
ylabel('amplitude')
xlabel('Seconds(s)')
title(title_str)
xlim([0 (4/f3)])

subplot(2,2,4)
plot(t,y4);
title_str = sprintf('Frequency at %d Hz', f4);
ylabel('amplitude')
xlabel('Seconds(s)')
title(title_str)
xlim([0 (4/f4)])

%========================Plotting the overall wave.========================
figure(2)
subplot(2,1,1)
plot(t,yt);
title('Overall resulting signal.')
xlabel('Seconds(s)')
xlim([0 4*scaler])

subplot(2,1,2)
semilogx(norm_x, abs(y_freq/(max(y_freq)))); %Only want the magnitude from the fft obj.
title('Frequency response of the input signal.')
xlabel('Frequency (Hz), Log Scale')
xlim([0 1.5*max_f])

cut_off_freq = f1/(Fs/2); %Have to normalize the cutoff frequency with Nyquist.
order = 512; %64 tap filter.

lp_fil = fir1(order,cut_off_freq); % Get the low pass filter output.

con = conv(yt,lp_fil); %Convolution.
lp_fil_freq = fft(lp_fil, nfft2); %Freq. convolution.
lp_fil_freq = lp_fil_freq(1:(nfft2/2));

freq_conv = lp_fil_freq.*y_freq;

%==========================Plotting the convolution results================
figure(3)
subplot(3,1,1)
semilogx(norm_x, abs(y_freq/max(y_freq)));
title('Frequency response of the input signal.')
xlabel('Frequency (Hz), Log Scale')
xlim([0 1.5*max_f])

subplot(3,1,2)
semilogx(norm_x, abs(lp_fil_freq));
title_str = sprintf('%d Tap FIR filter in frequency domain', order);
title(title_str)
xlabel('Frequency (Hz), Log Scale')
xlim([0 1.5*max_f])

subplot(3,1,3)
%Plotting the frequency domain convolution=================================
%semilogx(norm_x, abs(freq_conv)); %Plotting the remaining frequencies in a filtered wave.
%title('Convolution in frequency domain')
%xlabel('Frequency (Hz), Log Scale')
%xlim([0 1.5*max_f])

%Plotting the time domain convolution====================================
t_len = length(t);
plot(t, con(1:t_len));
title('Convolution in time domain')
ylabel('amplitude')
xlabel('Seconds(s)')
xlim([0 4/f1])

temp_arr = zeros(1, length(t));
temp_arr(1:length(lp_fil)) = lp_fil;

dataTable = table();
dataTable.time = transpose(t);
dataTable.fir = transpose(temp_arr);
dataTable.signal = transpose(yt);

writetable(dataTable, 'results.csv')
