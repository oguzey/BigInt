CC = g++

INCLUDES = -I./logger -I./logger/spdlog/include
LIBS = -pthread

CFLAGS = -std=c++11 -Wall -Wpedantic
LFLAGS =

COMMON_SRCS = $(wildcard ./src/*.cpp)

SRCS := $(COMMON_SRCS) main.cpp
OBJS = $(SRCS:.cpp=.o)

TEST_SRCS = $(wildcard ./tests/*.cpp) $(COMMON_SRCS)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

TARGET = App
TEST_TARGET = testApp

ifeq ($(MAKECMDGOALS),release)
	CFLAGS := $(CFLAGS) -O3
	TARGET := release$(TARGET)
endif

ifeq ($(MAKECMDGOALS),debug)
	CFLAGS := $(CFLAGS) -O0 -g3
	TARGET := debug$(TARGET)
endif

ifeq ($(MAKECMDGOALS),clean)
	TARGET := debug$(TARGET) release$(TARGET)
endif


.PHONY: release debug test clean

default: debug

release: $(TARGET)

debug: $(TARGET)

test: $(TEST_TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TEST_TARGET) $(TEST_OBJS) $(LFLAGS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	find ./ -name "*.o" | xargs rm -f
	rm -f $(TARGET) $(TEST_TARGET)
