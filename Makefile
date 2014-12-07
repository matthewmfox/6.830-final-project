# Makefile for Thesis Debugging
# Matt Fox - 10/8/14

CC = g++
CFLAGS = -l sqlite3
DEPS =
OBJ = db.o
OBJ2 = twitterStream.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

db: $(OBJ)
	g++ -o $@ $^ $(CFLAGS)
	
twitterStream: $(OBJ2)
	g++ -o $@ $^ $(CFLAGS)
