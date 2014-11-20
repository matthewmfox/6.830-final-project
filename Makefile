# Makefile for Thesis Debugging
# Matt Fox - 10/8/14

CC = g++
CFLAGS = -l sqlite3
DEPS =
OBJ = db.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

db: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)
