g++ -c runner.cpp
g++ -c mainC.cpp
g++ runner.o mainC.o -L../libraries -l:parameters.a -l:networking_library.a -o client
mv client ../executables
