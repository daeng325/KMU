    #include <stdio.h> 
    #include <stdlib.h> 
    #include <unistd.h> 
    #include <sys/types.h> 
    #include <sys/ipc.h> 
    #include <sys/sem.h> 
    #include <errno.h> 
    #include <string.h> 
    #include <time.h> 
    #define SEMPERM 0600 
    #define TRUE 1 
    #define FALSE 0 
     
    typedef union   _semun { 
                 int val; 
                 struct semid_ds *buf; 
                 ushort *array; 
                 } semun; 
    
     
    int initsem (key_t semkey, int n) { 
       int status = 0, semid; 
       if ((semid = semget (semkey, 1, SEMPERM | IPC_CREAT | IPC_EXCL)) == -1) 
       { 
           if (errno == EEXIST) 
                    semid = semget (semkey, 1, 0); 
       } 
       else 
       { 
           semun arg; 
           arg.val = n; 
           status = semctl(semid, 0, SETVAL, arg); 
       } 
       if (semid == -1 || status == -1) 
       { 
           perror("initsem failed"); 
           return (-1); 
       } 
       return (semid); 
    } 
     
    int p (int semid) { 
       struct sembuf p_buf; 
       p_buf.sem_num = 0; 
       p_buf.sem_op = -1; 
       p_buf.sem_flg = SEM_UNDO; 
       if (semop(semid, &p_buf, 1) == -1) 
       { 
          printf("p(semid) failed"); 
          exit(1); 
       } 
       return (0); 
    } 
     
    int v (int semid) { 
       struct sembuf v_buf; 
       v_buf.sem_num = 0; 
       v_buf.sem_op = 1; 
       v_buf.sem_flg = SEM_UNDO; 
       if (semop(semid, &v_buf, 1) == -1) 
       { 
          printf("v(semid) failed"); 
          exit(1); 
       } 
       return (0); 
    } 
     
     
    // Shared variable by file 
    void reset(char *fileVar) { 
    // fileVar라는 이름의 텍스트 화일을 새로 만들고 0값을 기록한다. 
     int nResult = access(fileVar, F_OK); 
     if(nResult == -1){ 
      FILE *n_file = fopen(fileVar, "w"); 
         fprintf(n_file, "0"); 
         fclose(n_file); 
     } 
    } 
     
    void Store(char *fileVar,int i,pid_t pid) { 
    // fileVar 화일 끝에 i 값을 append한다. +process id + time 
        FILE *out= fopen(fileVar, "a"); 
         
        time_t now_time = time(NULL); 
        struct tm tm= *localtime(&now_time); 
        int hour = tm.tm_hour; 
        int min = tm.tm_min; 
        int sec = tm.tm_sec; 
         
        fprintf(out, "\n%d-%d-%d %d %d", hour, min, sec, pid, i); 
        fflush(out); 
        fclose(out);     
    } 
     
    int Load(char *fileVar) { 
    // fileVar 화일의 마지막 값을 읽어 온다. 
        FILE *in = fopen(fileVar, "r"); 
        int result; 
        while (fscanf(in, "%d", &result) != EOF) 
        { 
        } 
        fflush(in); 
        fclose(in); 
        return result; 
    } 
     
    void add(char *fileVar,int i, pid_t pid) { 
    // fileVar 화일의 마지막 값을 읽어서 i를 더한 후에 이를 끝에 append 한다. 
        int last_num = Load(fileVar); 
        int result = last_num + i; 
        Store(fileVar,result, pid); 
    } 
     
    void sub(char *fileVar,int i, pid_t pid) { 
    // fileVar 화일의 마지막 값을 읽어서 i를 뺀 후에 이를 끝에 append 한다. 
        int last_num = Load(fileVar); 
        int result = last_num - i; 
        Store(fileVar, result, pid); 
    } 
     
    // Class Lock 
    typedef struct _lock {  
       int semid; 
    } Lock; 
     
    void initLock(Lock *l, key_t semkey) { 
       if ((l->semid = initsem(semkey,1)) < 0)     
       // 세마포를 연결한다.(없으면 초기값을 1로 주면서 새로 만들어서 연결한다.) 
          exit(1); 
    } 
     
    void Acquire(Lock *l) { 
       p(l->semid); 
    } 
     
    void Release(Lock *l) { 
       v(l->semid); 
    } 
     
    // Class CondVar 
    typedef struct _cond { 
       int semid; 
       char *queueLength; 
    } CondVar; 
     
    void initCondVar(CondVar *c, key_t semkey, char *queueLength) { 
       c->queueLength = queueLength; 
       reset(c->queueLength); // queueLength=0 
       if ((c->semid = initsem(semkey,0)) < 0)     
       // 세마포를 연결한다.(없으면 초기값을 0로 주면서 새로 만들어서 연결한다.) 
          exit(1);  
    } 
     
    void Wait(CondVar *c, Lock *lock, pid_t pid) { 
       add(c->queueLength,1, pid); 
       Release(lock); 
       p(c->semid); 
       Acquire(lock); 
    } 
     
    void Signal(CondVar *c,pid_t pid) { 
        if(Load(c->queueLength) >0){ 
            v(c->semid); 
            sub(c->queueLength,1,pid); 
        } 
    } 
     
    void Broadcast(CondVar *c,pid_t pid) { 
        while(Load(c->queueLength) >0){ 
            v(c->semid); 
            sub(c->queueLength,1,pid); 
        } 
    } 
    
     
    void Reader(CondVar *c_write, CondVar *c_read, Lock *lock,int i, int j, pid_t pid) { 
        sleep(i); 
     printf("\nprocess %d before critical section\n", pid); 
        Acquire(lock); 
     printf("process %d in critical section\n",pid); 
         
        while ((Load("AW") + Load("WW"))> 0) { // Is it safe to read? 
            add("WR",1,pid);// No. Writers exist 
            Wait(c_read,lock,pid); // Sleep on cond var 
            sub("WR",1,pid);// No longer waiting 
        } 
        add("AR",1,pid);// Now we are active! 
        Release(lock); 
     
        //AccessDbase(ReadOnly); 
        sleep(j); 
         
        Acquire(lock); 
        sub("AR",1,pid); 
        if (Load("AR") == 0 && Load("WW") > 0) 
            Signal(c_write,pid);  
     printf("process %d leaving critical section\n", pid); 
        Release(lock); 
     printf("process %d exiting\n",pid); 
    } 
     
     
    void Writer(CondVar *c_write, CondVar *c_read, Lock *lock, int i, int j,pid_t pid) {  
        sleep(i); 
     printf("\nprocess %d before critical section\n", pid); 
        Acquire(lock); 
     printf("process %d in critical section\n",pid); 
         
        while ((Load("AW") + Load("AR")) > 0) { // Is it safe to write? 
            add("WW",1,pid);// No. Active users exist  
            Wait(c_write,lock,pid);// Sleep on cond var  
            sub("WW",1,pid);// No longer waiting  
        } 
        add("AW",1,pid); 
        Release(lock); 
     
        //AccessDbase(ReadWrite); 
        sleep(j); 
         
        Acquire(lock); 
        sub("AW",1,pid); 
        if (Load("WW") > 0){  
            Signal(c_write,pid);  
        }  
        else if (Load("WR") > 0) {  
            Broadcast(c_read,pid);  
        } 
     printf("process %d leaving critical section\n", pid); 
        Release(lock);  
     printf("process %d exiting\n",pid); 
    }  
     
    int main(int argc, char* argv[]) { 
       key_t semkey = 0x200;  
       //  서버에서 작업할 때는 자기 학번 등을 이용하여 다른 사람의 키와 중복되지 않게 해야 한 다. 
       //  실행하기 전에 매번 세마포들을 모두 지우거나 아니면 다른 semkey 값을 사용해야 한다. 
       //  $ ipcs                 // 남아 있는 세마포 확인 
       //  $ ipcrm -s <semid>     // <semid>라는 세마포 제거 
     
       int semid; 
       pid_t pid; 
        
       Lock lock; 
        
       CondVar okToRead; 
       CondVar okToWrite; 
     
       pid = getpid(); 
       initLock(&lock,semkey); 
        
       initCondVar(&okToRead,0x201,"okToRead"); 
       initCondVar(&okToWrite,0x202,"okToWrite");    
        
        
       //printf("\nprocess %d before critical section\n", pid); 
       //Acquire(&lock);   // lock.Acquire() 
       //printf("process %d in critical section\n",pid); 
        
       //which operation (reader, writer) 
       char* op = argv[1]; 
       //how long sleepn 
       int sleep_one = atoi(argv[2]); 
       int sleep_two = atoi(argv[3]); 
        
       if(strcmp(op,"reader")==0){ 
               Reader(&okToWrite,&okToRead,&lock,sleep_one,sleep_two,pid);  
           } 
       else if(strcmp(op,"writer")==0){ 
               Writer(&okToWrite,&okToRead,&lock,sleep_one,sleep_two,pid); 
           } 
           else{ 
               printf("FAIL!!\n"); 
           } 
     
        
       //printf("process %d leaving critical section\n", pid); 
       //Release(&lock);   // lock.Release() 
       //printf("process %d exiting\n",pid); 
       //exit(0); 
       return 0; 
       } 
     
     
    
     
