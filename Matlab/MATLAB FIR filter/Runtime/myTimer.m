%this is a small function used to time how long each fir filter
%implementation takes using different sized signals.

timeArr = zeros(1, 6);
count = 1;

%Test with 100000 samples
tstart = tic;
FIR;
timerStop = toc(tstart);
timeArr(count) = timerStop;
count = count + 1;

%Test with 1000000 samples
tstart = tic;
FIR_1_000_000;
timerStop = toc(tstart);
timeArr(count) = timerStop;
count = count + 1;

%Test with 10000000 samples
tstart = tic;
FIR_10_000_000;
timerStop = toc(tstart);
timeArr(count) = timerStop;
count = count + 1;

%Test with 2^17 samples
tstart = tic;
FIR_2x2;
timerStop = toc(tstart);
timeArr(count) = timerStop;
count = count + 1;

%Test with 2^20 samples
tstart = tic;
FIR_2x3;
timerStop = toc(tstart);

timeArr(count) = timerStop;
count = count + 1;


%Test with 2^24 samples
tstart = tic;
FIR_2x4;
timerStop = toc(tstart);

timeArr(count) = timerStop;

%Write stuff to the table
timingTable = table();

timingTable.TestNum = transpose((1:count));
timingTable.ElapsedTime = transpose(timeArr);

writetable(timingTable, 'timing.csv')