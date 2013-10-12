all:	chordservice
	g++ chordservice -o all -lcrypto

chordservice:	ChordService.cpp ChordService.h
	g++ -c -g -o chordservice ChordService.cpp 
clean:
	rm -rf all chordservice 
