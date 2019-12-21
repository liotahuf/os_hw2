

all: Bank

# Environment for C++
CXX = g++
CXXFLAGS = -std=c++0x -Wall

SRC_GIVEN = Bank.cpp
EXTRA_DEPS = ATM.h

OBJ_GIVEN = $(patsubst %.cpp,%.o,$(SRC_GIVEN))
OBJ_BP = BankFunc.o
OBJ = $(OBJ_GIVEN) $(OBJ_BP)

#$(info OBJ=$(OBJ))

Bank: $(OBJ)
	$(CXX) -o $@ $(OBJ) -lpthread

BankFunc.o: ATM.cpp ATM.h


.PHONY: clean
clean:
	rm -f Bank $(OBJ)
