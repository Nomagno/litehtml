raybrowser: raybrowser.cpp
clean:
	rm -f *.o raybrowser

raybrowser: raybrowser.o raylib_container.o
	g++ -o raybrowser raybrowser.o raylib_container.o -llitehtml -lgumbo -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

raybrowser.o: raybrowser.cpp raylib_container.h
	g++ -c raybrowser.cpp

raylib_container.o: raylib_container.cpp  raylib_container.h
	g++ -c raylib_container.cpp
