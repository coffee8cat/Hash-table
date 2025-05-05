set datafile separator comma
set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
set output 'img/crc32_bucket_distribution.png'

set title "CRC32 Hash Bucket Distribution"
set xlabel "Bucket"
set ylabel "Count"
set grid
set boxwidth 0.8 relative
set style fill solid

set xrange [0:2000]
plot 'data/bucket_distribution.csv' using 1:2 skip 1 with boxes lc rgb "#406090" notitle


set datafile separator comma
set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
set output 'img/crc32_sizes_distribution.png'

set title "CRC32 Hash Sizes Distribution"
set xlabel "Size"
set ylabel "Count"
set grid
set boxwidth 0.8 relative
set style fill solid

set xrange [0:*]
plot 'data/sizes_distribution.csv' using 1:2 skip 1 with boxes lc rgb "#406090" notitle
