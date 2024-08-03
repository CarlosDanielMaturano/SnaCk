CC=$(clang)

build:
	- mkdir out
	clang -o out/main src/main.c

run:
	./out/main

all:
	make build 
	make run

