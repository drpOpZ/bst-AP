
CXX = g++
CXXFLAGS = -I include -Wall -Wextra -std=c++14 

SRC = src/main.cpp
HEADERS = include/bst.hpp

EXE = bst_test

.PHONY = $(EXE)

$(EXE): $(SRC) $(HEADERS)
	$(CXX) $(SRC) -o $@ $(CXXFLAGS)
