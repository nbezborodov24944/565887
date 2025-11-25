all:
	gcc t10.c -o t10
	./t10 ls -l
	./t10 sleep 1
	./t10 ls /nope
	./t10 sh -c "kill -9 $$"