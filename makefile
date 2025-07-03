all: compile run

compile: 
	g++ *.cpp -O2 -std=c++23


run:
	./a.out
