CC = clang++
CFLAGS = -l sqlite3 -ltwitcurl -std=c++11 -stdlib=libc++
DEPS =
OBJ = db.o
OBJ2 = twitterStream.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

db: $(OBJ)
	clang++ -o $@ $^ $(CFLAGS)
	
twitterStream: $(OBJ2)
	clang++ -o $@ $^ $(CFLAGS)
