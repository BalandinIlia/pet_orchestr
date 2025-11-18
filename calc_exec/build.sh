g++ -c main.cpp
g++ main.o -L../libraries -l:networking_library.a -l:networking_utils.a -l:logger.a -l:parameters.a -o calc_exec
mv calc_exec ../executables