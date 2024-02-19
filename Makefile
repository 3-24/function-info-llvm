ifeq (, $(shell which llvm-config))
$(error "No llvm-config in $$PATH")
endif

LLVMVER  = $(shell llvm-config --version 2>/dev/null | sed 's/git//' | sed 's/svn//' )
LLVM_MAJOR = $(shell llvm-config --version 2>/dev/null | sed 's/\..*//' )
LLVM_MINOR = $(shell llvm-config --version 2>/dev/null | sed 's/.*\.//' | sed 's/git//' | sed 's/svn//' | sed 's/ .*//' )
$(info Detected LLVM VERSION : $(LLVMVER))

CC=clang
CXX=clang++
CFLAGS=`llvm-config --cflags` -fPIC -ggdb -O0
AR=ar

CXXFLAGS=`llvm-config --cxxflags` -fPIC -ggdb -O0

MAKEFILE_PATH=$(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR:=$(dir $(MAKEFILE_PATH))

.PHONY : clean all print_function

all: print_function

print_function: lib/libprintfunc.so

lib/libprintfunc.so: src/print-function-pass.cpp
	mkdir -p lib
	$(CXX) $(CXXFLAGS) -shared $< -o $@

clean:
	rm -f lib/*.so lib/*.o log
