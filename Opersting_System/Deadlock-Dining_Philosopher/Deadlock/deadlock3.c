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

void initVar(char* fileVar){
//fileVar라는 텍스트 파일을 만들고 1값을 기록한다.
	int nResult = access(fileVar, F_OK);
	if(nResult == -1){
		FILE *n_file = fopen(fileVar, "w");
		fprintf(n_file, "1");
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

void Take_R1(pid_t pid, Lock *L1, CondVar *C1) {
   Acquire(L1);
   // R1이 1이면 젓가락이 있고 0이면 젓가락이 없어서 기다려야 함.
   while (Load("R1")==0) {
	   printf("\nprocess %d waits R1\n", pid);
        // print message: getpid()가 R1을 기다림.
        Wait(C1,L1,pid);
	   printf("\nprocess %d wakes up for R1", pid);
        // print message: getpid()가 R1을 기다리다가 깨어남.
       }
   Store("R1",0,pid); 
	printf("\nprocess %d gets R1", pid);
	// print message: getpid()가 R1을 가져옴.

   Release(L1);
}

void Take_R2(pid_t pid, Lock *L2, CondVar *C2) {
   Acquire(L2);
   while (Load("R2")==0){
	   printf("\nprocess %d waits R2\n", pid);
        Wait(C2,L2,pid);
	   printf("\nprocess %d wakes up for R2", pid);
      }
   Store("R2",0,pid); 
	printf("\nprocess %d gets R2", pid);
	
   Release(L2);
}

void Take_R3(pid_t pid, Lock *L3, CondVar *C3) {
   Acquire(L3);
   while (Load("R3")==0) {
	   printf("\nprocess %d waits R3\n", pid);
        Wait(C3,L3,pid);
	   printf("\nprocess %d wakes up for R3", pid);
       }
   Store("R3",0,pid); 
	printf("\nprocess %d gets R3", pid);

   Release(L3);
}


void Put_R1(pid_t pid, Lock *L1, CondVar *C1) {
   Acquire(L1);
   Store("R1",1,pid);
   Signal(C1,pid);
	printf("\nprocess %d waits R1", pid);
   // print message: getpid()가 R1을 기다림.
   Release(L1);
}

void Put_R2(pid_t pid, Lock *L2, CondVar *C2) {
   Acquire(L2);
   Store("R2",1,pid);
   Signal(C2,pid);
	printf("\nprocess %d waits R2", pid);
   Release(L2);
}

void Put_R3(pid_t pid, Lock *L3, CondVar *C3) {
   Acquire(L3);
   Store("R3",1,pid);
   Signal(C3,pid);
	printf("\nprocess %d waits R3", pid);
   Release(L3);
}

void Phil_C(pid_t pid,Lock *L1, Lock *L3, CondVar *C1, CondVar *C3) {
   Take_R3(pid,L3,C3);
	printf("\nprocess %d start thinking", pid);
   // print message: getpid()가 생각을 시작함.
   sleep(1); 
	printf("\nprocess %d stop thinking", pid);
   // print message: getpid()가 생각을 멈춤.
   Take_R1(pid,L1,C1);
	printf("\nprocess %d start eating", pid);
   // print message: getpid()가 식사를 시작함.
   sleep(1); 
	printf("\nprocess %d stop eating", pid);
   // print message: getpid()가 식사를 멈춤.
   Put_R3(pid,L3,C3);
   Put_R1(pid,L1,C1);
}

int main(){
	
	int semid;
	pid_t pid;
   
	Lock L1;
	Lock L2;
	Lock L3;
   
	CondVar C1;
	CondVar C2;
	CondVar C3;

	pid = getpid();
	initLock(&L1,0x200);
	initLock(&L2,0x201);
	initLock(&L3,0x202);
   
   initCondVar(&C1,0x203,"C1");
   initCondVar(&C2,0x204,"C2");
	initCondVar(&C3,0x205,"C3");
	
	initVar("R1");
	initVar("R2");
	initVar("R3");
	
	
	for(int i=0; i<100; i++){
		Phil_C(pid,&L1,&L3,&C1,&C3);
	}
	
	return 0;
}

