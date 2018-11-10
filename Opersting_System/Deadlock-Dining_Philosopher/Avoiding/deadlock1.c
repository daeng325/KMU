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
// fileVar라는 텍스트 파일을 만들고 0값을 기록한다.
	int nResult = access(fileVar,F_OK);
	if(nResult ==-1){ //그 파일이 존재하지 않으면
		FILE *n_file = fopen(fileVar, "w");
    		fprintf(n_file, "0");
    		fclose(n_file);
	}
}

void initVar(char* fileVar,int i){
//fileVar라는 텍스트 파일을 만들고 i값을 기록한다.
	int nResult = access(fileVar, F_OK);
	if(nResult == -1){
		FILE *n_file = fopen(fileVar, "w");
		fprintf(n_file, "%d",i);
		fclose(n_file);
	}
}

void Store(char *fileVar,int i,pid_t pid) {
// fileVar 파일 끝에 i 값을 append한다. + process id + time
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
// fileVar 파일의 마지막 값을 읽어온다.
    FILE *in = fopen(fileVar, "r");
    int result;
    while (fscanf(in, "%d", &result) != EOF)
    {
    }
    fflush(in);
    fclose(in);
    return result;
}

void add(char *fileVar, int i, pid_t pid){
	int last_num = Load(fileVar);
	int result = last_num + i;
	Store(fileVar, result, pid);
}

void sub(char *fileVar, int i, pid_t pid){
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



void Take_R1(pid_t pid, Lock *lock, CondVar *C1, char *count) {
   Acquire(lock);
   // R1이 1이면 젓가락이 있고 0이면 젓가락이 없어서 기다려야 함.
   while (Load("R1")==0) {
	   printf("\nprocess %d waits R1\n", pid);
        // print message: getpid()가 R1을 기다림.
       Wait(C1,lock,pid);
	   printf("\nprocess %d wakes up for R1", pid);
        // print message: getpid()가 R1을 기다리다가 깨어남.
       }
   
   //Avoiding();
   if(Load("counter")>1){
        add(count,1,pid);
   		sub("counter",1,pid);
   		Store("R1",0,pid); 
   }
   else if(Load("counter")==1){
   		int num_A = Load("counter_A");
   		int num_B = Load("counter_B");
   		int num_C = Load("counter_C");
   		if(num_A==1){
   			add("counter_A",1,pid);
   			sub("counter",1,pid);
   			Store("R1",0,pid); 
   			//A한테 R1을 준다.
   		}
   		else if(num_B==1){
   			add("counter_B",1,pid);
   			sub("counter",1,pid);
   			Store("R1",0,pid); 
   			//B한테 R1을 준다.
   		}
   		else if(num_C==1){
   			add("counter_C",1,pid);
   			sub("counter",1,pid);
   			Store("R1",0,pid); 
   			//C한테 R1을 준다.
   		}
   }
	printf("\nprocess %d gets R1", pid);
	// print message: getpid()가 R1을 가져옴.

   Release(lock);
}

void Take_R2(pid_t pid, Lock *lock, CondVar *C2, char *count) {
   Acquire(lock);
   while (Load("R2")==0){
	   printf("\nprocess %d waits R2\n", pid);
       Wait(C2,lock,pid);
	   printf("\nprocess %d wakes up for R2", pid);
      }
   
   //Avoiding();
   if(Load("counter")>1){
   		sub("counter",1,pid);
   		add(count,1,pid);
   		Store("R2",0,pid); 
   }
   else if(Load("counter")==1){
   		int num_A = Load("counter_A");
   		int num_B = Load("counter_B");
   		int num_C = Load("counter_C");
   		if(num_A==1){
   			add("counter_A",1,pid);
   			sub("counter",1,pid);
   			Store("R2",0,pid); 
   			//A한테 R2를 준다.
   		}
   		else if(num_B==1){
   			add("counter_B",1,pid);
   			sub("counter",1,pid);
   			Store("R2",0,pid); 
   			//B한테 R2를 준다.
   		}
   		else if(num_C==1){
   			add("counter_C",1,pid);
   			sub("counter",1,pid);
   			Store("R2",0,pid); 
   			//C한테 R2를 준다.
   		}
   }
	printf("\nprocess %d gets R2", pid);
	
   Release(lock);
}

void Take_R3(pid_t pid, Lock *lock, CondVar *C3, char *count) {
   Acquire(lock);
   while (Load("R3")==0) {
	   printf("\nprocess %d waits R3\n", pid);
       Wait(C3,lock,pid);
	   printf("\nprocess %d wakes up for R3", pid);
       }
   
   //Avoiding();
   if(Load("counter")>1){
   		sub("counter",1,pid);
   		add(count,1,pid);
   		Store("R3",0,pid); 
   }
   else if(Load("counter")==1){
   		int num_A = Load("counter_A");
   		int num_B = Load("counter_B");
   		int num_C = Load("counter_C");
   		if(num_A==1){
   			add("counter_A",1,pid);
   			sub("counter",1,pid);
   			Store("R3",0,pid); 
   			//Phil_A(); A한테 R3를 준다.
   		}
   		else if(num_B==1){
   			add("counter_B",1,pid);
   			sub("counter",1,pid);
   			Store("R3",0,pid); 
   			//B한테 R3를 준다.
   		}
   		else if(num_C==1){
   			add("counter_C",1,pid);
   			sub("counter",1,pid);
   			Store("R3",0,pid); 
   			//C한테 R3를 준다.
   		}
   }
	printf("\nprocess %d gets R3", pid);

   Release(lock);
}


void Put_R1(pid_t pid, Lock *lock, CondVar *C1, char *count) {
   Acquire(lock);
   Store("R1",1,pid);
   sub(count,1,pid);
   add("counter",1,pid);
   Signal(C1,pid);
	printf("\nprocess %d puts R1", pid);
   // print message: getpid()가 R1을 내려놓음.
   Release(lock);
}

void Put_R2(pid_t pid, Lock *lock, CondVar *C2, char *count) {
   Acquire(lock);
   Store("R2",1,pid);
   sub(count,1,pid);
   add("counter",1,pid);
   Signal(C2,pid);
	printf("\nprocess %d puts R2", pid);
   Release(lock);
}

void Put_R3(pid_t pid, Lock *lock, CondVar *C3, char *count) {
   Acquire(lock);
   Store("R3",1,pid);
   sub(count,1,pid);
   add("counter",1,pid);
   Signal(C3,pid);
	printf("\nprocess %d puts R3", pid);
   Release(lock);
}

void Phil_A(pid_t pid,Lock *lock, CondVar *C1, CondVar *C2) {
   Take_R1(pid,lock,C1,"counter_A");
	printf("\nprocess %d start thinking", pid);
   // print message: getpid()가 생각을 시작함.
   sleep(1); 
	printf("\nprocess %d stop thinking", pid);
   // print message: getpid()가 생각을 멈춤.
   Take_R2(pid,lock,C2,"counter_A");
	printf("\nprocess %d start eating", pid);
   // print message: getpid()가 식사를 시작함.
   sleep(1); 
	printf("\nprocess %d stop eating", pid);
   // print message: getpid()가 식사를 멈춤.
   Put_R1(pid,lock,C1,"counter_A");
   Put_R2(pid,lock,C2,"counter_A");
}

void Phil_B(pid_t pid, Lock *lock, CondVar *C2, CondVar *C3) {
   Take_R2(pid,lock,C2,"counter_B");
	printf("\nprocess %d start thinking", pid);
   // print message: getpid()가 생각을 시작함.
   sleep(1); 
	printf("\nprocess %d stop thinking", pid);
   // print message: getpid()가 생각을 멈춤.
   Take_R3(pid,lock,C3,"counter_B");
	printf("\nprocess %d start eating", pid);
   // print message: getpid()가 식사를 시작함.
   sleep(1); 
	printf("\nprocess %d stop eating", pid);
   // print message: getpid()가 식사를 멈춤.
   Put_R2(pid,lock,C2,"counter_B");
   Put_R3(pid,lock,C3,"counter_B");
}

void Phil_C(pid_t pid,Lock *lock, CondVar *C1, CondVar *C3) {
   Take_R3(pid,lock,C3,"counter_C");
	printf("\nprocess %d start thinking", pid);
   // print message: getpid()가 생각을 시작함.
   sleep(1); 
	printf("\nprocess %d stop thinking", pid);
   // print message: getpid()가 생각을 멈춤.
   Take_R1(pid,lock,C1,"counter_C");
	printf("\nprocess %d start eating", pid);
   // print message: getpid()가 식사를 시작함.
   sleep(1); 
	printf("\nprocess %d stop eating", pid);
   // print message: getpid()가 식사를 멈춤.
   Put_R3(pid,lock,C3,"counter_C");
   Put_R1(pid,lock,C1,"counter_C");
}

int main(int argc, char* argv[]){
	
	pid_t pid;
   
	Lock lock;
   
	CondVar C1;
	CondVar C2;
	CondVar C3;

	pid = getpid();
	
	initVar("R1",1);
	initVar("R2",1);
	initVar("R3",1);
	
	//int counter = 3;
	//int counter_A = 0;
	initVar("counter",3);
	initVar("counter_A",0);
	initVar("counter_B",0);
	initVar("counter_C",0);
	
	
	initLock(&lock,0x200);
   
   initCondVar(&C1,0x203,"C1");
   initCondVar(&C2,0x204,"C2");
	initCondVar(&C3,0x205,"C3");
	
	char* op = argv[1];
	

	if(strcmp(op,"A")==0){
         for(int i=0; i<100; i++){
			Phil_A(pid,&lock,&C1,&C2);
		}
	}
   else if(strcmp(op,"B")==0){
   		for(int i=0; i<100; i++){
			Phil_B(pid,&lock,&C2,&C3);
		}
	}
   else if(strcmp(op,"C")==0){
   		for(int i=0; i<100; i++){
			Phil_C(pid,&lock,&C1,&C3);
		}
	}
	else{
		printf("Fail!!");
	}
	
	return 0;
}
