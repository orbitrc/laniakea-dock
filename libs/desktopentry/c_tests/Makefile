default:
	gcc main.c -I../include -L../target/debug -ldesktopentry -o test

run:
	LD_LIBRARY_PATH=../target/debug ./test

clean:
	rm test
