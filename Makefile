ChordServer:	chordservice node
	g++ chordservice node -g -o ChordServer 
	g++ -std=c++11 -g -o Client Client.cpp

chordservice:	ChordService.cpp ChordService.h Node.h
	g++ -std=c++11 -c -g -o chordservice ChordService.cpp  
	
node:	Node.cpp Node.h 
	g++ -std=c++11 -c -g -o node Node.cpp
	
clean:
	rm -rf ChordServer chordservice node Client
