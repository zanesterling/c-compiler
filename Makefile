FLAGS := -std=c++17
objects := $(patsubst %.cpp,%.o,$(wildcard grammar/*.cpp))
headers := $(wildcard grammar/*.h)
CC  ?= g++ $(FLAGS)
CXX ?= g++ $(FLAGS)

CXXFLAGS=-std=c++17

.PHONY: run clean
run: a.out
	./a.out

runcomp: compiler.out
	./compiler.out test-cases/0001_minimal-program.c

compiler.out: parser_out.cpp
	g++ $(FLAGS) parser_out.cpp -o compiler.out

parser_out.cpp: a.out
	./a.out

clean:
	rm -f a.out $(objects)

a.out: $(objects) $(headers)
	g++ $(FLAGS) $(objects)
