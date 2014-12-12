all: malvin test

malvin:
	g++ malvin.cpp -std=c++11 -lexpat -pthread -lz -lprotobuf-lite -losmpbf -lz -lbz2 -pedantic -o malvin
	
test:
	./malvin debrecen.osm > route

clean:
	rm malvin
	
debug:	clean
	g++ malvin.cpp -std=c++11 -lexpat -ggdb -pthread -lz -lprotobuf-lite -losmpbf -lz -lbz2 -pedantic -o malvin
	gdb --args ./malvin debrecen.osm > route
