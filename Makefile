# Makefile for networkCounter
INCLUDES := Listener.hpp \
		IListenerInputHandler.hpp \
		CountHandler.hpp

SOURCES := main.cpp \
		Listener.cpp \
		CountHandler.cpp

OBJS := $(SOURCES:.cpp=.o)

all: networkCounter

networkCounter: $(OBJS)
	$(CC) $(CFLAGS) -std=gnu++17 -o $(LFLAGS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<
	

