all: build test1 test2 test3
build:
	gcc t10.c -o t10
test1:
	./t10 ls -l
test2:
	./t10 sleep 1
test3:
	-./t10 ls /nope
