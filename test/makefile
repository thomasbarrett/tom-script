export PATH := /usr/local/opt/llvm/bin:${PATH}
export LDFLAGS :=  -L/usr/local/opt/llvm/lib

DIR = $(patsubst src/%, obj/%, $(wildcard src/**))
SRC = $(wildcard src/**/*.cpp)
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))

SRC_OBJ = $(wildcard ../obj/AST/*.o) $(wildcard ../obj/Basic/*.o) $(wildcard ../obj/IR/*.o) $(wildcard ../obj/Parse/*.o) $(wildcard ../obj/Sema/*.o)


$(shell mkdir -p $(DIR))
$(shell mkdir bin > /dev/null)

CXX = clang++
CXXFLAGS = -std=c++14 -c -g -Wall -I../include -I/usr/local/opt/llvm/include

all: $(OBJ)
	clang++ -std=c++14 -g $^ $(SRC_OBJ) `llvm-config --cxxflags --ldflags --system-libs --libs core mcjit native` -lpthread -lgtest -lgtest_main -o bin/test

obj/%.o: src/%.cpp
	$(CXX) $< $(CXXFLAGS) -o $@

clean:
	rm -r obj
	rm -r bin
