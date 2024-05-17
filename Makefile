
all:
	cc hissy.c -o hissy.exe -Wall -pedantic -lncursesw
	./hissy.exe ${args}

test:
	cc test.c -o test.exe -Wall -pedantic -lncursesw
	./test.exe ${args}