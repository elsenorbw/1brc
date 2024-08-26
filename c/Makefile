

buffered: buffered.c
	gcc -W -Wall -O3 -o buffered buffered.c

simple: simple.c
	gcc -W -Wall -O3 -o simple simple.c

test: buffered
	time ./buffered | tee answer-buffered.txt
	hexdump -C answer-buffered.txt > answer-buffered-hex.txt


