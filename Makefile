all: server client

run: install
	./server

test: install
	gdb ./server

install:
	$(eval export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH)

server: src/server.c liblist.so
	gcc -g -o server src/server.c -L. -llist

liblist.so: src/list.o
	gcc -shared -o liblist.so src/list.o -fPIC

src/list.o: src/llist.c
	gcc -c -fpic src/llist.c -o src/list.o

client: src/client.c
	gcc -o client src/client.c

clean:
	rm -f client server src/list.o liblist.so
