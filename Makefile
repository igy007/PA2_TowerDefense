all: compile run

ALIAS = g++ -Wall -pedantic -std=c++11 -c


run:
	./igersdom


main.o: src/main.cpp src/NCExtension.h src/CObject.h src/exceptions.h
	$(ALIAS) -o main.o src/main.cpp


game.o: src/game.cpp src/game.h src/NCExtension.h src/exceptions.h src/CObject.h src/towers.h
	$(ALIAS) -o game.o src/game.cpp


NCExt.o: src/NCExtension.cpp src/NCExtension.h
	$(ALIAS) -o NCExt.o src/NCExtension.cpp


towers.o: src/towers.cpp src/towers.h
	$(ALIAS) -o towers.o src/towers.cpp


enemies.o: src/enemies.cpp src/enemies.h
	$(ALIAS) -o enemies.o src/enemies.cpp


compile: main.o game.o NCExt.o towers.o enemies.o
	g++ -o igersdom main.o game.o towers.o NCExt.o enemies.o -lncurses

clean:
	rm -r main.o game.o towers.o NCExt.o enemies.o igersdom
	rm -rf doc

doc: