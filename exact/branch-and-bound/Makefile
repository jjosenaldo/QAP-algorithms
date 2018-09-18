PROG = bin/exe
CC = g++
CPPFLAGS = -Wall -W -ansi -pedantic -g -O0 -std=c++11
INCLUDE_DIR = include
SRC_DIR = src
OBJS = hungarian.o branch_and_bound.o main.o

$(PROG) : $(OBJS)
	@echo "\nLinking...\n"
	$(CC) $(OBJS) -o $(PROG) $(LIBS)
	@echo "\nSuccessfully compiled!\n"

branch_and_bound.o : $(INCLUDE_DIR)/branch_and_bound.h $(SRC_DIR)/branch_and_bound.cpp $(SRC_DIR)/hungarian.cpp
	$(CC) $(CPPFLAGS) -I $(INCLUDE_DIR) -c $(SRC_DIR)/branch_and_bound.cpp

hungarian.o : $(INCLUDE_DIR)/hungarian.h $(SRC_DIR)/hungarian.cpp
	$(CC) $(CPPFLAGS) -I $(INCLUDE_DIR) -c $(SRC_DIR)/hungarian.cpp

main.o : $(INCLUDE_DIR)/branch_and_bound.h $(SRC_DIR)/main.cpp
	$(CC) $(CPPFLAGS) -I $(INCLUDE_DIR) -c $(SRC_DIR)/main.cpp 

clean:
	rm *.o