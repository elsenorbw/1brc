

CC = gcc
CFLAGS = -W -Wall -O3




HASH_TEST_OBJS = hash_test.o hash.o set.o 


mutex: mutex.c hash.o
	gcc -W -Wall -O3 -o mutex mutex.c hash.o 


mutex10: mutex.c hash.o
	gcc -W -Wall -O3 -D THREADS=10 -o mutex10 mutex.c hash.o 



testmutex: mutex
	time ./mutex | tee answer-mutex.txt
	hexdump -C answer-mutex.txt > answer-mutex-hex.txt


hash_test: $(HASH_TEST_OBJS)
	$(CC) $(CFLAGS) $(HASH_TEST_OBJS) -o $@
	./$@


testp5: posix5
	hyperfine ./posix5 2> posix5.stderr.txt > answer-posix5.txt
	diff answer-posix[45].txt

testp4: posix4
	time ./posix4 | tee answer-posix4.txt
	hexdump -C answer-posix4.txt > answer-posix4-hex.txt

testp3: posix3
	time ./posix3 | tee answer-posix3.txt
	hexdump -C answer-posix3.txt > answer-posix3-hex.txt


posix5: posix5.c hash.o
	gcc -W -Wall -O3 -o posix5 posix5.c hash.o 

posix4: posix4.c hash.o
	gcc -W -Wall -O3 -o posix4 posix4.c hash.o 

posix3: posix3.c hash.o
	gcc -W -Wall -O3 -o posix3 posix3.c hash.o 

posix3-dbg: posix3.c hash.o
	gcc -W -Wall -O3 -g -o posix3-dbg posix3.c hash.o 

posix3-profile: posix3.c hash-profile.o
	gcc -W -Wall -pg -l c_p -O3 -o posix3-profile posix3.c hash-profile.o 

posix2: posix2.c
	gcc -W -Wall -O3 -o posix2 posix2.c

posix1: posix1.c
	gcc -W -Wall -O3 -o posix1 posix1.c

mem-mapped: mem-mapped.c
	gcc -W -Wall -O3 -o mem-mapped mem-mapped.c

pure: pure.c
	gcc -W -Wall -O3 -o pure pure.c

pure2: pure2.c
	gcc -W -Wall -O3 -o pure2 pure2.c

pure3: pure3.c
	gcc -W -Wall -O3 -o pure3 pure3.c


profilep2: posix2
	perf record -a -g -F 99 ./posix2
	perf script report flamegraph

hash-profile.o: hash.c hash.h
	gcc -c -pg -W -Wall -O3 -o hash-profile.o hash.c

profile: posix3-profile
	touch gmon.out
	touch gmon.out.old
	rm gmon.out.old
	mv gmon.out gmon.out.old
	./posix3-profile
	./posix3-profile
	./posix3-profile
	gprof ./posix3-profile gmon.out > gmon.txt
	cat gmon.txt


profile2: posix3-dbg
	valgrind 

profileme: pure.c
	gcc -W -Wall -O3 -pg -o profile-me pure.c


simple: simple.c
	gcc -W -Wall -O3 -o simple simple.c


testmm: mem-mapped
	time ./mem-mapped | tee answer-mmap.txt
	hexdump -C answer-mmap.txt > answer-mmap-hex.txt


test1: pure
	time ./pure | tee answer-pure.txt
	hexdump -C answer-pure.txt > answer-pure-hex.txt


test2: pure2
	time ./pure2 | tee answer-pure2.txt
	hexdump -C answer-pure2.txt > answer-pure2-hex.txt

test3: pure3
	time ./pure3 | tee answer-pure3.txt
	hexdump -C answer-pure3.txt > answer-pure3-hex.txt

testp1: posix1
	time ./posix1 | tee answer-posix1.txt
	hexdump -C answer-posix1.txt > answer-posix1-hex.txt

testp2: posix2
	time ./posix2 | tee answer-posix2.txt
	hexdump -C answer-posix2.txt > answer-posix2-hex.txt

test: test1 test2 test3


flamep4: posix4
	sudo perf record -F 99 -g -- ./posix4
	sudo chown bmw:bmw perf.data
	perf script | FlameGraph/stackcollapse-perf.pl > out.perf-folded
	FlameGraph/flamegraph.pl out.perf-folded > perf-posix4.svg


obtain_flamegraph_software:
	git clone https://github.com/brendangregg/FlameGraph


%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@



