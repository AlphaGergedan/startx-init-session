all: main #keypress

#keypress:
	#g++ src/keypress.cpp -lncurses -o keypress

main:
	g++ src/main.cpp -lncurses -lmenu -o main

clean:
	rm ./main #./keypress
