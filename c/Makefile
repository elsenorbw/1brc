

buffered: buffered.c
	gcc -W -Wall -O3 -o buffered buffered.c


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


profileme: buffered.c
	gcc -W -Wall -O3 -pg -o profile-me buffered.c


simple: simple.c
	gcc -W -Wall -O3 -o simple simple.c

test: buffered
	time ./buffered | tee answer-buffered.txt
	hexdump -C answer-buffered.txt > answer-buffered-hex.txt


