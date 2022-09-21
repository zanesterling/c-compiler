FLAGS := -std=c++17
objects := $(patsubst %.cpp,%.o,$(wildcard grammar/*.cpp))
headers := $(wildcard grammar/*.h)
CC  ?= g++ $(FLAGS)
CXX ?= g++ $(FLAGS)

CXXFLAGS=-std=c++17

.PHONY: run clean
run: a.out
	./a.out

clean:
	rm -f a.out $(objects)

a.out: $(objects) $(headers)
	g++ $(FLAGS) $(objects)
