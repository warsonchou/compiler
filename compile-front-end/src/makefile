# DO NOT DELETE
OBJS = inter.o Lexer.o  Tokenizer.o View.o  parser.o
CC = g++
DEBUG = -g
CFLAGS = -c

aql : $(OBJS)
	$(CC) $(OBJS) -o AQL

Lexer.o : Lexer.h Lexer.cpp
	$(CC) $(CFLAGS) Lexer.cpp

Tokenizer.o : Tokenizer.h Tokenizer.cpp
	$(CC) $(CFLAGS) Tokenizer.cpp

View.o : View.h View.cpp
	$(CC) $(CFLAGS) View.cpp

inter.o : inter.h inter.cpp 
	$(CC) $(CFLAGS) inter.cpp

regex.o : regex.cpp
	$(CC) $(CFLAGS) regex.cpp

Output : 
	$(CC) $(CFLAGS) Output.cpp

parser.o : parser.h parser.cpp
	$(CC) $(CFLAGS) parser.cpp

clean:
	\rm *.o *~ AQL