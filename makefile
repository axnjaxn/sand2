CFLAGS = -g

all: sand2

sand2:	sand2.lex.o sand2.tab.o
	$(CXX) -g -o $@ sand2.tab.o sand2.lex.o -lm

sand2.lex.o: sand2.lex.c sand2.tab.hh sand2-ctx.h
	$(CXX) -c $< -g

sand2.tab.o: sand2.tab.cc sand2-ctx.h
	$(CXX) -c $< -g

sand2.lex.c: sand2.l
	flex -osand2.lex.c sand2.l

sand2.tab.cc sand2.tab.hh: sand2.yy
	bison -vd sand2.yy

clean:
	rm -f sand2 \
	sand2.lex.c sand2.tab.c sand2.tab.cc sand2.tab.hh \
	sand2.lex.o sand2.tab.o sand2.output \
	location.hh position.hh stack.hh *~

run: sand2 elements.dat
	./sand2 elements.dat