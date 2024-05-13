
all:
	cc hissy.c -o hissy -Wall -pedantic -lncursesw
	./hissy.exe ${args}