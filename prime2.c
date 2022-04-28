#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/times.h>
#include <sys/stat.h>
#include <time.h>
#include <math.h>

#define YES 1
#define NO  0

int prime(int n){
        int i=0, limitup=0;
        limitup = (int)(sqrt((float)n));

        if (n==1) return(NO);
        for (i=2 ; i <= limitup ; i++)
                if ( n % i == 0) return(NO);
        return(YES);
}

int main(int argc, char *argv[]){

        clock_t start = clock();
        int lb=0, ub=0, i=0;

        lb=atoi(argv[1]);
        ub=atoi(argv[2]);
        int step = atoi(argv[3]);
        if ( ( lb<1 )  || ( lb > ub ) ) {
                printf("%d %d\n", lb, ub);
                printf("usage: prime1 lb ub\n");
                exit(1); }
        
        char *s = malloc(sizeof(char) * (ub - lb + 2) * 30);
        
        for (i=lb ; i <= ub ; i++){
                
                clock_t t = clock();

                if ( prime(i)==YES ){
                        t = clock() - t;
                        sprintf(s + strlen(s), "%d %lf ", i, ((double)t / CLOCKS_PER_SEC) * 1000);
                }
        }

        char *fifo = "myfifo";
        int fd = open(fifo, O_WRONLY);
        write(fd, s, strlen(s) + 1);
        close(fd);

        start = clock() - start;
        double total = ((double)start/CLOCKS_PER_SEC) * 1000;

        char *worktime = "worktime", *ss = malloc(80);
        sprintf(ss, "%lf", total);
        fd = open(worktime, O_WRONLY);
        write(fd, ss, 80);
        close(fd);
        
        char *to_ins = "to_ins";
        int fd1 = open(to_ins, O_WRONLY);
        char *send = malloc(240);
        sprintf(send, "Time for W%d: %lf msecs\n", lb / step, total);
        write(fd1, send, 240);
        close(fd1);
        return 0;
}
