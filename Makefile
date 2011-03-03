
# Global variables:
#

BIN = sudoku.elf
OBJ = main.o solver.o generator.o

CC = g++-4.5

CFLAGS = -Wall -O3 -std=c++0x
LFLAGS = -Wall -O3


# Build targets:
#

$(BIN): $(OBJ)
	$(CC) $(LFLAGS) -o $(BIN) $(OBJ)

clean:
	rm -f *.o *~

all: $(BIN)


# Build rules:
#

main.o: main.cpp solver.h generator.h sudoku.h
	$(CC) $(CFLAGS) -o $@ -c main.cpp

solver.o: solver.cpp solver.h sudoku.h
	$(CC) $(CFLAGS) -o $@ -c solver.cpp

generator.o: generator.cpp generator.h solver.h sudoku.h
	$(CC) $(CFLAGS) -o $@ -c generator.cpp

