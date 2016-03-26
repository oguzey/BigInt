CC = g++

INCLUDES = -I./logger -I./logger/spdlog/include
LIBS = -pthread

CFLAGS = -std=c++11 -Wall -Wpedantic
LFLAGS =

SRCS = $(wildcard ./src/*.cpp) main.cpp
OBJS = $(SRCS:.cpp=.o)

TEST_SRCS = $(wildcard ./tests/*.cpp)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

TARGET = mainProg
TEST_TARGET = testProg

.PHONY: release debug test clean

default: debug

all: release debug test

release: $(TARGET)
	CFLAGS = $(CFLAGS) -O3
	echo -e "cflags for release setup\n"

debug: $(TARGET)
	CFLAGS = $(CFLAGS) -O0 -g3
	echo -e "cflags for debug setup\n"

test: $(TEST_TARGET)
	CFLAGS = $(CFLAGS) -O0 -g3
	echo -e "cflags for test setup\n"

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)
	echo -e "targed built\n"

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TEST_TARGET) $(TEST_OBJS) $(LFLAGS) $(LIBS)
	echo -e "test target built\n"

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@
	echo -e "build source $<\n"

clean:
	find ./ -name "*.o" | xargs rm
	rm $(TARGET) $(TEST_TARGET)
