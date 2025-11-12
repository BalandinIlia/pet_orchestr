g++ -c messages.cpp
g++ -c utils.cpp
ar rcs networking_library.a messages.o utils.o
mv networking_library.a ../libraries