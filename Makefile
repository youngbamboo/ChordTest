all:	chordservice
	g++ chordservice node -o all -lcrypto

chordservice:	ChordService.cpp ChordService.h Node.h
	g++ -c -g -o chordservice ChordService.cpp 
	
node:	Node.cpp Node.h 
	g++ -c -g -o node Node.cpp
	
clean:
	rm -rf all chordservice 
