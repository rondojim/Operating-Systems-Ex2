    /* ------------------------------------------------------- Make the executables
    if(fork() == 0){    
        execl("/usr/bin/gcc", "gcc", "/home/rondojim/Desktop/Project2/prime1.c", "-o", "prime1", (char*) NULL);
        return 0;
    }

    if(fork() == 0){
        execl("/usr/bin/gcc", "gcc", "/home/rondojim/Desktop/Project2/prime2.c", "-o", "prime2", "-lm", (char*) NULL);
        return 0;
    }
    
    if(fork() == 0){
        execl("/usr/bin/gcc", "gcc", "/home/rondojim/Desktop/Project2/prime3.c", "-o", "prime3", "-lm", (char*) NULL);
        return 0;
    }

    */ -------------------------------------------------------