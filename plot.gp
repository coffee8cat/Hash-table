set datafile separator comma
set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
set output 'murmur_bucket_distribution.png'

set title "Murmur64A Hash Bucket Distribution"
set xlabel "Bucket"
set ylabel "Count"
set grid
set boxwidth 0.8 relative
set style fill solid

plot 'data/bucket_distribution.csv' using 1:2 skip 1 with boxes lc rgb "#406090" notitle


set datafile separator comma
set terminal pngcairo size 800,600 enhanced font 'Verdana,10'
set output 'murmur_sizes_distribution.png'

set title "Murmur64A Hash Sizes Distribution"
set xlabel "Size"
set ylabel "Count"
set grid
set boxwidth 0.8 relative
set style fill solid

plot 'data/sizes_distribution.csv' using 1:2 skip 1 with boxes lc rgb "#406090" notitle
