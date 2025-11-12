g++ -c solver.cpp
g++ -c host.cpp
ar rcs math_core.a solver.o host.o
mv math_core.a ../libraries
