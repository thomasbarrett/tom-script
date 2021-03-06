export PATH := /usr/local/opt/llvm/bin:${PATH}
export LDFLAGS :=  -L/usr/local/opt/llvm/lib

DIR = $(patsubst src/%, obj/%, $(wildcard src/**))
SRC = $(wildcard src/**/*.cpp)
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))

$(shell mkdir -p $(DIR))
$(shell mkdir bin > /dev/null)

CXX = clang++
CXXFLAGS = -std=c++14 -c -g -Wall -pedantic -Iinclude -I/usr/local/opt/llvm/include

all: $(OBJ)
	clang++ -std=c++14 -g $^ `llvm-config --cxxflags --ldflags --system-libs --libs all` -o bin/tomscript

obj/%.o: src/%.cpp
	$(CXX) $< $(CXXFLAGS) -o $@

clean:
	rm -r obj
	rm -r bin
