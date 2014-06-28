all: catan

catan: catan.cpp catan.h UI.h UIBase.h
	g++ catan.cpp -o catan -lncurses

clean:
	rm -rf *.o catan


