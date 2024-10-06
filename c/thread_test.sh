

for THREADCOUNT in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18; do
  echo "Testing with $THREADCOUNT"
	gcc -W -Wall -O3 -D THREADS=$THREADCOUNT -o mutex$THREADCOUNT mutex.c hash.o 
  hyperfine -w 2 --export-json mutex-$THREADCOUNT.json ./mutex$THREADCOUNT  
done

