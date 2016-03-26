CC = g++

INCLUDES = -I./logger -I./logger/spdlog/include
LIBS = -pthread

CFLAGS = -std=c++11 -Wall -Wpedantic
LFLAGS =

COMMON_SRCS = $(wildcard ./src/*.cpp)
TARGET = App
SRCS =

ifeq ($(MAKECMDGOALS),release)
	CFLAGS := $(CFLAGS) -O3
	TARGET := release$(TARGET)
	SRCS := $(COMMON_SRCS) main.cpp
endif

ifeq ($(MAKECMDGOALS),debug)
	CFLAGS := $(CFLAGS) -O0 -g3 -DDEBUG_ON
	TARGET := debug$(TARGET)
	SRCS := $(COMMON_SRCS) main.cpp
endif

ifeq ($(MAKECMDGOALS),test)
	CFLAGS := $(CFLAGS) -O0 -g3 -DDEBUG_ON
	TARGET := test$(TARGET)
	SRCS := $(COMMON_SRCS) $(wildcard ./tests/*.cpp)
endif

ifeq ($(MAKECMDGOALS),clean)
	TARGET := debug$(TARGET) release$(TARGET) test$(TARGET)
endif


OBJS = $(SRCS:.cpp=.o)

.PHONY: release debug test clean

default: debug

release: $(TARGET)

debug: $(TARGET)

test: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	find ./ -name "*.o" | xargs rm -f
	rm -f $(TARGET) $(TEST_TARGET)
