set datafile separator comma
set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
set output 'crc32_distribution.png'

set title "CRC32 Hash Distribution"
set xlabel "Bucket"
set ylabel "Count"
set grid
set boxwidth 0.8 relative
set style fill solid

plot 'data/distribution.csv' using 1:2 skip 1 with boxes lc rgb "#406090" notitle
