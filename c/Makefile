

mem-mapped: mem-mapped.c
	gcc -W -Wall -O3 -o mem-mapped mem-mapped.c

pure: pure.c
	gcc -W -Wall -O3 -o pure pure.c

pure2: pure2.c
	gcc -W -Wall -O3 -o pure2 pure2.c

pure3: pure3.c
	gcc -W -Wall -O3 -o pure3 pure3.c


profile: profileme
	touch gmon.out
	touch gmon.out.old
	rm gmon.out.old
	mv gmon.out gmon.out.old
	./profile-me
	./profile-me
	./profile-me
	gprof ./profile-me gmon.out > gmon.txt
	cat gmon.txt


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


test: test1 test2 test3



