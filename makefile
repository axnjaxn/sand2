CFLAGS = -g -O3 -Wno-unused-result

ifndef MINGW
LFLAGS = -g `sdl2-config --libs` -lfl
EXT = 
else
LFLAGS = -g `sdl2-config --libs` -lfl
EXT = .exe
endif

all: sand2$(EXT) 

sand2$(EXT): sand2.lex.o sand2.tab.o s2struct.o elements.o world.o pixelrenderer.o osd.o bbc_font.o main.o
	$(CXX) -g -o $@ \
		sand2.tab.o sand2.lex.o s2struct.o elements.o world.o \
		pixelrenderer.o osd.o bbc_font.o main.o $(LFLAGS)

main.o: main.cpp s2struct.h sand2.tab.h elements.h
	$(CXX) -c $< $(CFLAGS)

elements.o: elements.cpp elements.h s2struct.h
	$(CXX) -c $< $(CFLAGS)

world.o: world.cpp world.h elements.h
	$(CXX) -c $< $(CFLAGS)

osd.o: osd.cpp osd.h
	$(CXX) -c $< $(CFLAGS)

s2struct.o: s2struct.c s2struct.h
	$(CC) -c $< $(CFLAGS)

.cpp.o:
	$(CXX) -c $< $(CFLAGS)

sand2.lex.o: sand2.lex.c sand2.tab.h s2struct.h
	$(CC) -c $< $(CFLAGS)

sand2.tab.o: sand2.tab.c s2struct.h
	$(CC) -c $< $(CFLAGS)

sand2.lex.c: sand2.l
	flex -osand2.lex.c sand2.l

sand2.tab.c sand2.tab.h: sand2.y
	bison -d sand2.y

clean:
	rm -f sand2 *.o *~ \
	sand2.lex.c sand2.tab.c sand2.tab.h

run: sand2 elements2.dat
	./sand2 elements2.dat