all: catan

catan: catan.cpp catan.h UI.h UIBase.h comms.h
	g++ catan.cpp -o catan -lncurses -g

clean:
	rm -rf *.o catan


