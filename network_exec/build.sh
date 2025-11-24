g++ -c clientInstance.cpp
g++ -c main_network.cpp
g++ clientInstance.o mainS_network.o -L../libraries -l:networking_library.a -l:networking_utils.a -l:logger.a -l:parameters.a -o network_exec
mv network_exec ../executables
