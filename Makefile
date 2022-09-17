FLAGS := -std=c++17
objects := $(patsubst %.cpp,%.o,$(wildcard grammar/*.cpp))
CC  ?= g++ $(FLAGS)
CXX ?= g++ $(FLAGS)

CXXFLAGS=-std=c++17

.PHONY: run
run: a.out
	./a.out

a.out: $(objects)
	g++ $(FLAGS) $(objects)
