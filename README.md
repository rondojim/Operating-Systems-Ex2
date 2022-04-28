# Operating-Systems-Ex2
Find primes in interval [N, M] with multiple processes

Using a tree of processes we find the primes in the interval [N, M]. 

We use functions like: exec*(), mkfifo(), pipe(), open(), close(), read(), write(),
poll(), wait(), kill(), exit()


./myprime -l lb -u ub -w NumofChildren

Where NumofChildren is the number of intervals each process will break its interval.
