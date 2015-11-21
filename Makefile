CC = clang++ -std=c++11

Page: Page.o
	$(CC) -o Page Page.o

Page.o: Page.cc
	$(CC) -c Page.cc

clean:
	rm -f Page.o Page
