g++ -c controls.cpp
g++ -c pch.cpp
ar rcs parameters.a pch.o controls.o
echo parameters.a created
mv parameters.a ../libraries
