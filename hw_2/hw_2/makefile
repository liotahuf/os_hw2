

all: Bank

# Environment for C++
CXX = g++
CXXFLAGS = -std=c++0x -Wall

SRC_GIVEN = Bank.cpp
EXTRA_DEPS = ATM.h

OBJS = Bank.o ATM.o




# Creating the  executable
Bank: $(OBJS)
	$(CXX) -o Bank $(OBJS) -lpthread 
# Creating the object files
Bank.o: Bank.cpp ATM.h
ATMS.o: ATM.cpp ATM.h



.PHONY: clean
clean:
	rm -f Bank $(OBJS)
