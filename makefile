CFLAGS = -g

all: sand2

sand2:	sand2.lex.o sand2.tab.o
	$(CC) -g -o $@ sand2.tab.o sand2.lex.o -lm

sand2.lex.o: sand2.lex.c sand2.tab.hh sand2-ctx.h
	$(CC) -c $< -g

sand2.tab.o: sand2.tab.cc sand2-ctx.h
	$(CC) -c $< -g

sand2.lex.c: sand2.l
	flex -osand2.lex.c sand2.l

sand2.tab.c sand2.tab.h: sand2.y
	bison -d sand2.y

clean:
	rm -f sand2 \
	sand2.lex.c sand2.tab.c sand2.tab.cc sand2.tab.hh \
	sand2.lex.o sand2.tab.o sand2.output *~

run: sand2 elements.dat
	./sand2 elements.dat