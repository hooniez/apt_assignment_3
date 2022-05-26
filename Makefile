.default: all

all: scrabble

clean:
	rm -rf scrabble *.o *.dSYM

scrabble: tile.o node.o linkedlist.o player.o game.o main.o board.o tilebag.o utils.o filemanager.o commandHandler.o Dictionary.o CoordinatedTile.o WordBuilder.o MapBuilder.o SortedMapBuilder.o GreedyMapBuilder.o AdjacentTile.o Word.o
	g++ -Wall -Werror -std=c++14 -g -o $@ $^
#g++ -Wall -Werror -std=c++14 -g -O -o $@ $^

%.o: %.cpp
#g++ -Wall -Werror -std=c++14 -g -O -c $^
	g++ -Wall -Werror -std=c++14 -g -c $^