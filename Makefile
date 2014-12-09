CC = clang++
CFLAGS = -l sqlite3 -ltwitcurl -std=c++11 -stdlib=libc++# -SQLITE_THREADSAFE=2
DEPS =
INC = -I/libtwitcurl/ -I/libtwitcul/include/curl
OBJ = db.o
OBJ2 = twitterStream.o
OBJ3 = sDB.o

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

db: $(OBJ)
	clang++ -o $@ $^ $(CFLAGS)

sDB: $(OBJ3)
	clang++ -o $@ $^ $(CFLAGS)
	
twitterStream: $(OBJ2)
	clang++ -o $@ $^ $(CFLAGS)
