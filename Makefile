CXX = g++
CC = gcc
CFLAGS = -Wall -Werror -Wno-unused-variable
CXXFLAGS = -Wall -Werror -Wno-unused-variable

all: markov

markov: markov.o lex.yy.o
	$(CXX) -g -std=c++11 markov.o lex.yy.o -ll -o markov

markov.o: markov.cpp lexer.h
	$(CXX) -g -c -std=c++11 markov.cpp -o markov.o

lex.yy.o: lex.yy.c lexer.h
	$(CC) -g -c -std=c99 lex.yy.c -o lex.yy.o

lex.yy.c lexer.h: lexer.l
	flex --header=lexer.h lexer.l

.PHONY: clean

clean:
	rm -rf markov *.o lex.yy.c lexer.h
