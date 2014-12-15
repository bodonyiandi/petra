all: clean petra test

petra:
	g++ petra.cpp -std=c++11 -lexpat -pthread -lz -lprotobuf-lite -losmpbf -lz -lbz2 -pedantic -o petra
	
test:
	./petra debrecen.osm 21.658961 47.514571 21.604907 47.549508 > route

clean:
	rm petra
	
debug:	clean
	g++ petra.cpp -std=c++11 -lexpat -ggdb -pthread -lz -lprotobuf-lite -losmpbf -lz -lbz2 -pedantic -o petra
	gdb --args ./petra debrecen.osm 47.2100 21.9909 47.6659 21.6524 > route
