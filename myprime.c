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
#include <math.h>

int main(int argc, char *argv[]){
    if(argc != 7){
        printf("Usage: ./myprime -l lb -u ub -w NumofChildren\n");
        exit(1);
    }


    int lo = -1, hi = -1, NumofChildren = -1, i;

    for(i=1; i<=5; i+=2){
        if(argv[i][1] == 'l') lo = atoi(argv[i + 1]);
        if(argv[i][1] == 'u') hi = atoi(argv[i + 1]);
        if(argv[i][1] == 'w') NumofChildren = atoi(argv[i + 1]);
    }

    char *results = "result_fifo";
    char *fifo = "myfifo";
    char *worktime = "worktime";
    char *to_ins = "to_ins";
    char *to_par = "to_par";
    char *maximum = "maximum";
    char *minimum = "minimum";

    mkfifo(results, 0666);
    mkfifo(fifo, 0666);
    mkfifo(worktime, 0666);
    mkfifo(maximum, 0666);
    mkfifo(minimum, 0666);
    mkfifo(to_ins, 0666);
    mkfifo(to_par, 0666);

    char *last = malloc(sizeof(char) * (int)((hi - lo + 2) / log(hi - lo + 2)) * 30);
    char *work_times = malloc(sizeof(char) * 40 * (NumofChildren + 1) * (NumofChildren + 1));
    int step = (hi - lo + 1) / NumofChildren;
    int l = lo, h = -1, lb, ub;

    lb = lo, ub = hi;
    int prime_algo = 0;

    pid_t pid;
    
    double resmin = 10000, resmax = -10000;

    for(; l <= hi; l += step){
        int flago = 0;
        if(l + step - 1 < hi){
            h = l + step - 1;
            if(hi - h < step) h = hi, flago = 1;
        }
        else h = hi;

        if(fork() == 0){    
            lo = l, hi = h;
            step = (hi - lo + 1) / NumofChildren;
            char *string =  malloc(sizeof(char) * (h - l + 2) * 30 * (NumofChildren + 1));

            int ll = l, hh = h;

            double mxt = -10000, mnt = 10000;

            char *sstring = malloc(sizeof(char) * 80 * (NumofChildren + 1));

            for(; ll <= hi; ll += step){
                int flag = 0;
                if(ll + step - 1 < hi){
                    hh = ll + step - 1;
                    if(hi - hh < step) hh = hi, flag = 1;
                }
                else hh = hi;
                prime_algo++;
                if(prime_algo > 3) prime_algo = 1;
                if(fork() == 0){

                    char lb[30], ub[30], arg0[30], arg1[30];
                    char w[30];

                    sprintf(arg1, "./prime%d", prime_algo);
                    sprintf(lb, "%d", ll);
                    sprintf(ub, "%d", hh);
                    sprintf(w, "%d", step);
                    char *args[] = {arg1, lb, ub, w, NULL};
                    if(fork() == 0)
                        execvp(args[0], args);
                    else
                        exit(1);  
                }
                else{
                    int xx = open(fifo, O_RDONLY);
                    int yy = open(worktime, O_RDONLY);
                    int zz = open(to_ins, O_RDONLY);

                    wait(NULL);
                    int fd1 = xx;
                    int SIZE = sizeof(char) * (hh - ll + 2) * 30;
                    char *ss = malloc(SIZE);
                    read(fd1, ss, SIZE);
                    if(strlen(ss))
                        sprintf(string + strlen(string), "%s", ss);
                    close(fd1);

                    fd1 = yy;
                    char *wt = malloc(80);
                    read(fd1, wt, 80);
                    close(fd1);

                    double ct;
                    sscanf(wt, "%lf", &ct);
                    if(mxt < ct) mxt = ct;
                    if(mnt > ct) mnt = ct;

                    fd1 = zz;
                    char *sent = malloc(240);
                    read(fd1, sent, 240);
                    close(fd1);
                    
                    sprintf(sstring + strlen(sstring), "%s", sent);
                    
                }
                if(flag) break;
            }
            
            int fd2 = open(results, O_WRONLY);
            write(fd2, string, strlen(string) + 1);
            close(fd2);

            char *csx = malloc(80), *csn = malloc(80);
            sprintf(csx, "%lf", mxt);
            sprintf(csn, "%lf", mnt);

            fd2 = open(maximum, O_WRONLY);
            write(fd2, csx, 80);
            close(fd2);

            fd2 = open(minimum, O_WRONLY);
            write(fd2, csn, 80);
            close(fd2);

            fd2 = open(to_par, O_WRONLY);
            write(fd2, sstring, sizeof(char) * (h - l + 2) * 30 * (NumofChildren + 1));
            close(fd2);
            exit(1);
        }
        else{
            int xx = open(results, O_RDONLY);
            int yy = open(maximum, O_RDONLY);
            int zz = open(minimum, O_RDONLY);
            int ww = open(to_par, O_RDONLY);
            wait(NULL);

            int fd3 = xx;
            int SIZE = sizeof(char) * (h - l + 2) * 30 * (NumofChildren + 1);
            char *string =  malloc(SIZE);
            read(fd3, string, SIZE);
            sprintf(last + strlen(last), "%s", string);
            close(fd3);

            fd3 = ww;
            char *sent = malloc(sizeof(char) * (h - l + 2) * 30 * (NumofChildren + 1));
            read(fd3, sent, sizeof(char) * (h - l + 2) * 30 * (NumofChildren + 1));
            close(fd3);
            sprintf(work_times + strlen(work_times), "%s", sent);
            double x;
            char *num = malloc(80);
            fd3 = yy;
            read(fd3, num, 80);
            sscanf(num, "%lf", &x);
            if(x > resmax) resmax = x;
            close(fd3);

            fd3 = zz;
            read(fd3, num, 80);
            sscanf(num, "%lf", &x);
            if(x < resmin) resmin = x;
            close(fd3);
        }
        if(flago) break;
    }
    printf("Primes in [%d, %d] are: %s\n", lb, ub, last);
    printf("Min Time for Workers : %lf msecs\n", resmin);
    printf("Max Time for Workers : %lf msecs\n", resmax);
    printf("%s", work_times);
    return 0;
}