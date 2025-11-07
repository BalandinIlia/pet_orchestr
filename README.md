There is an article at LinkedIn on this project. Here is a copy of the article (pictures and videos are omitted due to plain text format).


Introduction
This article describes my pet project in C++. The project is focused on networking, TCP sockets, multithreading. I have never worked with these technologies before the project, so I studied them on my own within the project.
 
The article is organized in the following sections:
	Math Task: briefly introduces the mathematical problem the project is based on
	Main Project Feature and Demo: introduces how the project looks from the user’s point of view. Also presents a demo of working project.
	Client-Server Architecture: explains how server and client(s) communicate over TCP and presents my mini-protocol over TCP
	Code Architecture: presents project code architecture; how the project is divided into modules; purpose of each module and how modules interact
	Run-Time Architecture: presents run-time structure: what threads are running, what tasks they execute, and how they interact
	Learnings: an overview of things I studied during the project
	How to run: explains how to run the project at your computer
Math Task
The math task is to decompose a positive (only positive, zero is excluded) integer number in a sum of different pows of 3, of to find out that the decomposition is impossible. For example 1 is decomposable: 1=3^0, 85 is decomposable: 85=3^4+3^1+3^0. On the other hand, 6 is not decomposable: 6=3^1+3^1, but both pows are the same 3^1.
The algorithm for the decomposition executes very quickly, so I introduced an artificial delay into it to simulate complex calculations. One of the project goals is to practice structure for complex computations. Due to delay each decomposition is calculated in 5 seconds.
Main Project Features and Demo
	A server and a client are two different applications.
	The server has hardcoded IP-address: 127.0.0.1 (loop-back) and TCP port 5555.
	The client sends to the server a number they want to decompose and receive from the server decomposition or answer, that decomposition is impossible. The client measures the time between the request and the answer.
	The user must type “a” and press Enter as a signal that they want to input a number.
	One server can serve multiple clients simultaneously.
	Server can serve multiple requests from the same client simultaneously: for example, if the client sent a request to the server and immediately sent another request, they get answers for both requests. Calculations for each request start immediately as it is received. Requests don’t stay in a queue.
	Server caches request answers. For example, if a client sends a request with a number which has already been sent (even by another client), they get answer immediately without any calculations.
	The user can just close the client window. The server handles this correctly and can serve other clients after this.

The source code is available at: BalandinIlia/pet_TCP_multithreading: -  In the code I don’t check correctness of each step, for example I just connect client to server assuming it will connect correctly. For a production project, of course, each step must be checked, but this is just a small pet study project.
Client-Server Architecture
Client and server communicate by TCP sockets which provide 2 byte streams: one from the client to the server and one from the server to the client. The client and the server use these streams to exchange messages in the following format:
First byte of the message (from TCP socket) is always message type. So application reads one byte from the socket, defines message type and acts differently for different message types. There are 3 message types:
	Request: a request from the client to the server. The message contains 10 bytes. So, the application reads these 10 bytes. First 2 bytes are unique request id. Last 8 bytes are a number for decomposition.
	AnswerYes: answer that decomposition is possible. First byte is the number of decomposition components (there are always no more than several dozens of components, so 1 byte is enough). Further 2 bytes contain unique request id and remaining bytes contain decomposition components (each component takes 8 bytes)
	AnswerNo: answer that decomposition is impossible. It contains 2 bytes with unique request id.

 
Code Architecture
The project consists of the following modules:
	Server: this module contains code for server part. It manages incoming connections from clients and their requests. This module manages “administration”. It does not do mathematical calculations.
	Math_Core: exposes a host to the server module. The host performs calculations and stores cached answers. The host allows to ask a decomposition for particular number, but decides itself if calculations are necessary, or the number is already cached. The host is thread-safe: it can be safely called from different threads. The host does not know anything about clients, or request ids. It only gets numbers for decomposition. Making math_code a separate module ensures modularity and shared cache for all clients.
	Client: code for client part.
	Networking. This module is shared between the client and the server modules. This module contains two components:
	Utility functions for sending/receiving data through sockets. The system send/recv functions don’t guarantee to send/receive all bytes. For example, one can call recv function for 10 bytes, but have only 5 bytes sent. In order to avoid this I wrote custom send/receive function which grant that they send/receive all bytes.
	Functions for encoding/decoding messages. This functions are shared between the client and the server modules and this ensures that the client and the server use the same custom protocol over TCP, - that “they speak the same language”
	Controls: contains some control elements, for example algorithm delay, or server TCP port.
Run-Time Architecture
Server run-time architecture
The server starts the main thread. The main thread listens for incoming connections. As soon as the main thread receives an incoming connection and establishes a socket for particular client, it creates a separate thread for the client and continues listening for incoming connections from other clients. This design allows to serve several clients simultaneously and perform calculations for them simultaneously: each client is served in their own thread and the main thread continues listening for new clients.
Client thread works the following way: it listens to incoming requests. As soon as it receives a request, it creates a separate request thread to serve the request (perform calculations and send the answer). This design allows to serve several requests from the same client simultaneously and perform calculations for several requests simultaneously. Each request is served in its own thread and the client thread continues listening to new requests. On the other hand, this design introduces a challenge: several concurrent requests threads send data to the same client through the same socket, which can cause data mixture. In order to prevent this I introduced a mutex for the socket. Once a request thread is ready to send an answer, it locks the mutex and unlocks it only when the data is completely sent. 
Video
Request threads call math host for decompositions. Math host can work in many threads simultaneously. This design provides better performance: several requests can be in the process of calculating simultaneously. However map with cached answers is protected by a mutex: only one thread can access it at any point in time. If I don’t do this, threads can spoil the cache by concurrent modifications.
Client run-time architecture
In runtime client consists of the following main parts:
	Send thread: receives requests from the user and send them to the server.
	Receive thread: receives answers from the server and shows them to the user.
	Inner table: all requests are registered here by id.
	Console mutex: prevents texts of send and receive threads from mixturing.
	Table mutex: prevents concurrent access to the inner table.
Learnings
Through the project I learned/practiced the following things:
	TCP sockets: I studied TCP sockets and how they work from scratch + I practiced them. Before the project I only had general knowledge of protocol stack and TCP place and function within the stack.
	Multithreading: I read about threads and mutexes several years ago, however it is my first hands-on practice with them.
	Data alignment: I studied that multi-byte types can’t be placed in any place in memory. For example, int which is 4-byte type must have address multiple to 4. For example, if I read/ write int at addresses like 4,8,12, it is ok. However, if I do the same at, for example, address 2, it causes undefined behaviour. This knowledge was critical for message serialization, since there are neighbour fields of different sizes in the network messages.
	ABI (Application Binary Interface): I studied at example of bind function. There is a special structure passed to the bind function as pointer. The reason for this is the following: the address is passed to the operating system core and the core reads the structure bytes just as they are in the memory. This is ABI, - an agreement on particular binary structure and purpose of each byte in the structure. To practice ABI I have even created my own version of binding (it is in a separate branch of the repository). In my version I don’t use the standard structure. Instead, I studied its binary format through looking into system files and I create binary buffer for operational system core “manually”, by assigning value to each byte individually.
	Big/small endian: I knew it before, just refreshed it.
	Types uint8_t, uint16_t, …. These types are used for networking protocols since they have the same size and binary format at all platforms. They are like a “universal language” for platforms.
How To Run
There are two ways to run the project at your computer:
	Download the project from BalandinIlia/pet_TCP_multithreading: -; compile it in Visual Studio (pet_project.sln file); this will generate two exe-files: server.exe and client.exe
	Alternatively you can download exe-files from Workflow runs · BalandinIlia/pet_TCP_multithreading as artifacts of the last run
