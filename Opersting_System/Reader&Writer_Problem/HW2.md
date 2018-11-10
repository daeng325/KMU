## HW2: Reader & Writer Problem

�������� �̿��Ͽ� Lock�� Condition Variable�� ����� ������ Mesa Style Monitor�� Reader & Writer Problem�� �����Ͻÿ�.

  

- Lock ���� �� ��Ʈ ([���� ������](https://sites.google.com/a/cs.kookmin.ac.kr/os/homework-1/synchronization-by-semaphore))

�ʱⰪ�� 1�� �������� ����� P()�� Acquire()�� V()�� Release()�� Ȱ���Ѵ�.

���� �� mutex ��� Lock�� �ʿ��ϸ�

    Lock mutex;
    
    // �����Ͽ� Lock �ν��Ͻ��� ���� ��üó�� ����ϴµ�
    
    ....
    
    initLock(&mutex, semkey);
    
    // �ݵ�� �ʱ�ȭ�� �ؾ��ϰ� �� �� �������� Ŀ�� �ȿ� ����� ����.
    
    // �� method �Լ��� ������ ���� ù��° �Ķ���ͷ� �ٲپ� ȣ���Ѵ�.
    
    Acquire(&mutex); // mutex.Acquire()
    
    Release(&mutex); // mutex.Release()
    
      
    
    // Class Lock
    
    typedef struct _lock {
    
    int semid; // ������ 1���� ����/��� �� waiting queue�� ���±��� ��� ǥ���ȴ�.
    
    } Lock;
    
      
    
    initLock(Lock *l, key_t semkey) {
    
    if ((l->semid = initsem(semkey,1)) < 0)
    
    // �������� �����Ѵ�.(������ �ʱⰪ�� 1�� �ָ鼭 ���� ���� �����Ѵ�.)
    
    exit(1);
    
    }
    
      
    
    Acquire(Lock *l) {
    
    p(l->semid);
    
    }
    
      
    
    Release(Lock *l) {
    
    v(l->semid);
    
    }

  

- Condition Variable ����� ��Ʈ

�������� waiting queue�� ����ִ����� ���������� �˱� ���� ������ ī���͸� �����Ѵ�.

�� ī���ʹ� method �Լ��� ��� �����Ǿ�� �ϴµ� ���⼭�� ȭ�� ����(fileVar)�� �����ϱ�� �Ѵ�.

  

    // Shared variable by file
    
      
    
    initVar(char *fileVar) {
    
    // fileVar��� �̸��� �ؽ�Ʈ **ȭ���� ������** ���� ����� 0���� ����Ѵ�.
    
    }
    
      
    
    Store(char *fileVar,int i) {
    
    // fileVar ȭ�� ���� i ���� append�Ѵ�.
    
    }
    
      
    
    int Load(char *fileVar) {
    
    // fileVar ȭ���� ������ ���� �о� �´�.
    
    }
    
      
    
    add(char *fileVar,int i) {
    
    // fileVar ȭ���� ������ ���� �о i�� ���� �Ŀ� �̸� ���� append �Ѵ�.
    
    }
    
      
    
    sub(char *fileVar,int i) {
    
    // fileVar ȭ���� ������ ���� �о i�� �� �Ŀ� �̸� ���� append �Ѵ�.
    
    }

  

- Reader, Writer�� �����ϸ� �����ϴ� 4���� ī����(AR,AW,WR,WW)�� 4���� ȭ�Ͽ� ���(fileVar)�ϱ�� �Ѵ�.

�� �� ȭ���� �ʱⰪ�� 0�̸� ���� ���� �� ���� ���� ���� �߰��Ͽ� ����Ѵ�.

�� �� ��ϵ� �ð� �� ����� ���μ����� ID(pid, getpid()�� ȹ��)�� �Բ� ����Ѵ�.

�� 4���� ī���Ͱ��� ��Ȳ�� �°� �����ϴ� ���� �� ����ȭ�ϴ� ���� �����̴�.

������ ���� �� 4���� ȭ���� �����Ͽ� �ð� ������ �����ϸ� ���ൿ�� �߻��� ��Ȳ�� �м��� �� �ִ�.

  

- reader, writer ����

��: reader 3 8 // 3�� �Ŀ� 8�ʰ� ����

�ٷ� ������� �ʰ� �־��� �ð� ��ŭ �����Ǿ��ٰ� �����Ѵ�. (�������� ���� sleep(3);)

������ �а� ���� ���� �ƴ϶� �޼����� ����ϰ� �־��� �ð���ŭ sleep(8); �Ѵ�.

���⼭ ����ϴ� sleep()�� �ð� ������ �������� �� ���̴�.

�� ���� �߿��� ���� �� �ð���ŭ �ϰ� �ִ� ������ �����Ѵ�.

  

- ���� �� �м�

$ reader 1 5 &

reader 2 5 &

writer 3 3 &

reader 4 5 &

reader 5 3 &

writer 6 3 &

  

���� ���� ������ �� ��ü ����ð��� �� ���̸� ���� ���� �ʰ� �����°��� ���� ����� ������� �����Ͻÿ�.

(����ð� - �ð� ������ ���� ���Ե� sleep()���θ� ���, 4�� ī���� ȭ�Ͽ� �����ϴ� �ð��� ����)

  

- ����!

1) �Ź� ���� ���� ���������� ��� ������ �Ѵ�.

$ ipcs // ���� �ִ� ������ Ȯ��(3��)

$ ipcrm -s semid // semid��� ������ ����

  

2) �Ź� ���� ���� ��� fileVar ȭ�ϵ鵵 �����.

  

3) �� ���������� �Ʒ��� ���� �Ѱ��� Lock�� �ΰ��� Condition Variable �� ����� ���ؼ� �� 3���� �������� ����� �Ǵµ� �� �� 3�� ��� semkey ���� �޶�� �Ѵ�.

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
