#!/bin/bash

export CPLUS_INCLUDE_PATH=/home/oleg/Projects/KPI/BigInt/logger/spdlog/include
mkdir -p build && cd build && cmake .. && make $1 && cd ..

