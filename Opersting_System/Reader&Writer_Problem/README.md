## HW2: Reader & Writer Problem

세마포를 이용하여 Lock과 Condition Variable을 만들어 구성된 Mesa Style Monitor로 Reader & Writer Problem을 구현하시오.

  

- Lock 만들 기 힌트 ([참고 페이지](https://sites.google.com/a/cs.kookmin.ac.kr/os/homework-1/synchronization-by-semaphore))

초기값이 1인 세마포를 만들어 P()는 Acquire()로 V()는 Release()로 활용한다.

예를 들어서 mutex 라는 Lock이 필요하면

    Lock mutex;
    
    // 선언하여 Lock 인스턴스를 만들어서 객체처럼 사용하는데
    
    ....
    
    initLock(&mutex, semkey);
    
    // 반드시 초기화를 해야하고 이 때 세마포는 커널 안에 만들어 진다.
    
    // 각 method 함수는 다음과 같이 첫번째 파라미터로 바꾸어 호출한다.
    
    Acquire(&mutex); // mutex.Acquire()
    
    Release(&mutex); // mutex.Release()
    
      
    
    // Class Lock
    
    typedef struct _lock {
    
    int semid; // 세마포 1개로 열림/잠김 및 waiting queue의 상태까지 모두 표현된다.
    
    } Lock;
    
      
    
    initLock(Lock *l, key_t semkey) {
    
    if ((l->semid = initsem(semkey,1)) < 0)
    
    // 세마포를 연결한다.(없으면 초기값을 1로 주면서 새로 만들어서 연결한다.)
    
    exit(1);
    
    }
    
      
    
    Acquire(Lock *l) {
    
    p(l->semid);
    
    }
    
      
    
    Release(Lock *l) {
    
    v(l->semid);
    
    }

  

- Condition Variable 만들기 힌트

세마포의 waiting queue가 비어있는지를 간접적으로 알기 위해 별도의 카운터를 관리한다.

이 카운터는 method 함수에 모두 공유되어야 하는데 여기서는 화일 변수(fileVar)로 구현하기로 한다.

  

    // Shared variable by file
    
      
    
    initVar(char *fileVar) {
    
    // fileVar라는 이름의 텍스트 **화일이 없으면** 새로 만들고 0값을 기록한다.
    
    }
    
      
    
    Store(char *fileVar,int i) {
    
    // fileVar 화일 끝에 i 값을 append한다.
    
    }
    
      
    
    int Load(char *fileVar) {
    
    // fileVar 화일의 마지막 값을 읽어 온다.
    
    }
    
      
    
    add(char *fileVar,int i) {
    
    // fileVar 화일의 마지막 값을 읽어서 i를 더한 후에 이를 끝에 append 한다.
    
    }
    
      
    
    sub(char *fileVar,int i) {
    
    // fileVar 화일의 마지막 값을 읽어서 i를 뺀 후에 이를 끝에 append 한다.
    
    }

  

- Reader, Writer가 협력하며 관리하는 4개의 카운터(AR,AW,WR,WW)는 4개의 화일에 기록(fileVar)하기로 한다.

즉 각 화일의 초기값은 0이며 값이 변할 때 마다 변한 값을 추가하여 기록한다.

이 때 기록된 시간 및 기록한 프로세스의 ID(pid, getpid()로 획득)도 함께 기록한다.

이 4개의 카운터값을 상황에 맞게 변경하는 것을 잘 동기화하는 것이 관건이다.

실행이 끝난 후 4개의 화일을 통합하여 시간 순으로 정렬하면 실행동안 발생한 상황을 분석할 수 있다.

  

- reader, writer 구현

예: reader 3 8 // 3초 후에 8초간 읽음

바로 실행되지 않고 주어진 시간 만큼 지연되었다가 실행한다. (시작하자 마자 sleep(3);)

실제로 읽고 쓰는 것이 아니라 메세지만 출력하고 주어진 시간만큼 sleep(8); 한다.

여기서 사용하는 sleep()은 시간 지연을 목적으로 한 것이다.

즉 뭔가 중요한 일을 그 시간만큼 하고 있는 것으로 가정한다.

  

- 실행 및 분석

$ reader 1 5 &

reader 2 5 &

writer 3 3 &

reader 4 5 &

reader 5 3 &

writer 6 3 &

  

위와 같이 실행할 때 전체 실행시간이 몇 초이며 누가 가장 늦게 끝나는가를 실제 실행된 기록으로 설명하시오.

(실행시간 - 시간 지연을 위해 삽입된 sleep()으로만 계산, 4개 카운터 화일에 접근하는 시간은 무시)

  

- 주의!

1) 매번 실행 전에 세마포들을 모두 지워야 한다.

$ ipcs // 남아 있는 세마포 확인(3개)

$ ipcrm -s semid // semid라는 세마포 제거

  

2) 매번 실행 전에 모든 fileVar 화일들도 지운다.

  

3) 이 숙제에서는 아래와 같이 한개의 Lock과 두개의 Condition Variable 을 만들기 위해서 총 3개의 세마포를 만들게 되는데 이 때 3개 모두 semkey 값이 달라야 한다.

> Lock mutex;
> 
> CondVar OkToRead, OkToWrite;
> 
> ...
> 
> initLock(&mutex, 0x200);
> 
> ...
> 
> initCondVar(&OkToRead, 0x201, ...);
> 
> initCondVar(&OkToWrite, 0x202, ...);
> 
> ...
