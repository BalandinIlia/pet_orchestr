g++ -c clientInstance.cpp
g++ -c logger.cpp
g++ -c mainS.cpp
g++ clientInstance.o logger.o mainS.o -L../libraries -l:parameters.a -l:networking_library.a -l:math_core.a -o server
mv server ../executables
