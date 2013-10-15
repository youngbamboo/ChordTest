ChordServer:	chordservice node
	g++ chordservice node -o ChordServer 
	g++ -std=c++11 -o Client Client.cpp

chordservice:	ChordService.cpp ChordService.h Node.h ClientMessage.h
	g++ -std=c++11 -c -g -o chordservice ChordService.cpp  
	
node:	Node.cpp Node.h 
	g++ -std=c++11 -c -g -o node Node.cpp
	
clean:
	rm -rf ChordServer chordservice node Client
