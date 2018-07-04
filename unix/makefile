CC=g++
CXXFLAGS+= -Wall -Wpedantic -fPIC -Os -pipe
LDFLAGS+= -s

all: DefaultInterfaceIP

verbose: DefaultInterfaceIP-verbose

DefaultInterfaceIP: DefaultInterfaceIP.o main.o

DefaultInterfaceIP-verbose: DefaultInterfaceIP-verbose.o main.o

.PHONY: clean
clean:
	rm -rf *.o
