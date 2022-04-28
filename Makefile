all: myprime prime1 prime2 prime3

myprime: myprime.c
	 gcc myprime.c -o myprime -lm

prime1: prime1.c
	gcc prime1.c -o prime1 -lm

prime2: prime2.c
	gcc prime2.c -o prime2 -lm

prime3: prime3.c
	gcc prime3.c -o prime3 -lm

clean:
	rm -f prime1 prime2 prime3 myprime a.out *.o
	rm myfifo maximum minimum result_fifo to_ins to_par worktime
