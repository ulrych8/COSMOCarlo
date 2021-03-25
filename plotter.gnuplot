set title " Time depending on nbVoters and nbCandidates "
set ylabel "nbVoters" offset 2, 0, 0 
set xlabel "nbCandidates" offset -4, 0, 0
set zlabel "Time (s)" offset -2, 0, 0

splot 'durationData.txt' using 1:2:3 '%lf,%lf,%lf,%lf' with points palette pointsize 3 pointtype 7 title ""
