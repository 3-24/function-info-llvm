CXX=clang++

.PHONY: all clean simple

all: simple

simple:
	gclang++ -O0 -g simple.cc -o simple
	get-bc simple
	opt -enable-new-pm=0 -load ../lib/libprintfunc.so -PrintFunc simple.bc -o simple.bc | diff - simple.out || (echo "Test failed" && exit 1)
clean:
	rm -f simple simple.bc