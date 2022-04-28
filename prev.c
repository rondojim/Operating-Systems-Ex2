#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char *argv[]){
    if(argc != 7){
        printf("Usage: ./myprime -l lb -u ub -w NumofChildren\n");
        exit(1);
    }

    char *results = "result_fifo";
    char *last;

    int lo = -1, hi = -1, NumofChildren = -1, i;

    for(i=1; i<=5; i+=2){
        if(argv[i][1] == 'l') lo = atoi(argv[i + 1]);
        if(argv[i][1] == 'u') hi = atoi(argv[i + 1]);
        if(argv[i][1] == 'w') NumofChildren = atoi(argv[i + 1]);
    }

    int step = (hi - lo + 1) / NumofChildren;
    int l = lo, h = -1;
    int prime_algo = 0;

    pid_t pid;

    for(; l <= hi; l += step){
        if(l + step - 1 < hi) h = l + step - 1;
        else h = hi;

        if((pid = fork()) == 0)
            break;
        else
            wait(NULL);
    }
    
    if(pid == 0 && l <= h){
        lo = l, hi = h;
        step = (hi - lo + 1) / NumofChildren;
        char *string =  malloc(sizeof(char) * (h - l + 2) * 30 * (NumofChildren + 1));

        for(; l <= hi; l += step){
            int flag = 0;
            if(l + step - 1 < hi){
                h = l + step - 1;
                if(hi - h < step) h = hi, flag = 1;
            }
            else h = hi;

            printf("%d %d\n", l, h);
            if(fork() == 0){
                prime_algo++;
                if(prime_algo > 3) prime_algo = 1;

                printf("Leaf: %d %d, with prime algo: %d\n", l, h, prime_algo);

                char lb[30], ub[30], arg0[30], arg1[30];

                sprintf(arg1, "./prime%d", prime_algo);
                sprintf(lb, "%d", l);
                sprintf(ub, "%d", h);

                char *args[] = {arg1, lb, ub, NULL};
                if(fork() == 0)
                    execvp(args[0], args);
                else
                    exit(1);  
            }
            else{
                wait(NULL);
                char *fifo = "myfifo";
                int fd1 = open(fifo, O_RDONLY);
                int SIZE = sizeof(char) * (h - l + 2) * 30;
                char *ss = malloc(SIZE);
                read(fd1, ss, SIZE);
                if(strlen(ss)){
                    printf("%s [%s]\n", string, ss);
                    sprintf(string + strlen(string), "%s", ss);
                }
                close(fd1);
            }
            if(flag) break;
        }

        int fd2 = open(results, O_WRONLY);
        write(fd2, string, strlen(string) + 1);
        printf("-------------%s\n", string);
        exit(1);
    }
    else{
        wait(NULL);
        int fd3 = open(results, O_RDONLY);

        int SIZE = sizeof(char) * (h - l + 2) * 30 * (NumofChildren + 1);
        char *string =  malloc(SIZE);
        read(fd3, string, SIZE);
        last = malloc(sizeof(char) * (h - l + 2) * 30 * (NumofChildren + 1) * (NumofChildren + 1));
        sprintf(last, "%s", string);
    }
    printf("last: [%s]\n", last);
    return 0;
}