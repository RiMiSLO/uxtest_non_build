TARGET = mi

LIBS = -lm

ifeq ($(OS),Windows_NT)
LIBS := $(LIBS) -lws2_32
endif

CC = gcc
CONTROL = ../control
INCLUDES = $(CONTROL)
CFLAGS = -g -Wall $(addprefix -I, $(INCLUDES))

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c)) $(patsubst %.cpp, %.o, $(wildcard $(CONTROL)/*.cpp))
HEADERS = $(wildcard *.h) $(wildcard $(CONTROL)/*.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o $(CONTROL)/*.o
	-rm -f $(TARGET)
