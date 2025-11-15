g++ -c clientInstance.cpp
g++ -c mainS.cpp
g++ clientInstance.o mainS.o -L../libraries -l:logger.a -l:networking_utils.a -l:networking_library.a -l:math_core.a -l:parameters.a -o network_exec
mv network_exec ../executables
