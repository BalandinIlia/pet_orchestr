g++ -c main_cache.cpp
g++ main_cache.o -L../libraries -l:networking_library.a -l:networking_utils.a -l:logger.a -l:parameters.a -o cache_exec
mv cache_exec ../executables