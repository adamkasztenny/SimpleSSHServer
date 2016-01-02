# SimpleSSHServer

A simple SSH-like server written in C. Uses standard Linux/Unix system calls to fork off a new bash process and run it on the server machine, and redirect its output to the client.

Example of running the server:
./simplesshserver localhost 7000 &

And to connect to it as a client:
nc -v localhost 7000

Please note that this server is not secure in any way.
