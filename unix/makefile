CC?=g++
CXXFLAGS+= -Wall -Wpedantic -fPIC -Os -pipe
LDFLAGS+= -s -lstdc++
OBJS= main.o DefaultInterfaceIP.o DefaultInterfaceIP-verbose.o

all: defaultinterfaceip

defaultinterfaceip: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf *.o defaultinterfaceip
