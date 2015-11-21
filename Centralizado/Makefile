CC = clang++ -std=c++11

PageRank: PageRank.o
	$(CC) -o PageRank PageRank.o

PageRank.o: PageRank.cc
	$(CC) -c PageRank.cc

clean:
	rm -f PageRank.o PageRank
