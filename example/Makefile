CC=g++
CFLAGS=-I.

all: zmq_pub zmq_sub

zmq_pub: zmq_pub.cpp
	g++ -o zmq_pub zmq_pub.cpp -std=gnu++11 -lzmq 

zmq_sub: zmq_sub.cpp
	g++ -o zmq_sub zmq_sub.cpp -std=gnu++11 -lzmq 