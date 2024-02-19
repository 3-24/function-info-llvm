set -e
set -o xtrace

rm -f main main.bc main_func.bc main_func

gclang -O0 -g  main.cc -o main 
get-bc main

opt -enable-new-pm=0 -load ../lib/libprintfunc.so --PrintFunc -o main_func.bc < main.bc
# llvm-dis-13 main_func.bc
# clang++ main_func.bc -o main_func
# ./main_func
