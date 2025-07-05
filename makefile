all: compile run

compile: 
	clang++ *.cpp -O2 -std=c++23


run:
	./a.out
