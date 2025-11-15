g++ -c send_receive.cpp
g++ -c make_socket.cpp
ar rcs networking_utils.a send_receive.o make_socket.o
mv networking_utils.a ../libraries