CC=clang++ -std=c++11 -Wall -L/home/alexander/zmq/lib -I/home/alexander/zmq/include
LDFLAGS=-lpthread -lzmqpp -lzmq -lsodium

all: Client Broker

Client: Client.cc
	$(CC) -o Client Client.cc $(LDFLAGS)

Broker: Broker.cc
	$(CC) -o Broker Broker.cc $(LDFLAGS)

	
clean:
	rm -f Client Broker