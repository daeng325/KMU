
## HW3: Deadlock - Dining Philosopher

1. Dining Philosopher 문제를  세마포를  이용하여  구현하시오.

-   첨부 화일 참고

    
   - 철학자  세명 A,B,C
    
   - 젓가락  세개 R1,R2,R3
    
   - 각  젓가락을 Mutual Exclusive 하게  만들기  위한 Lock L1, L2, L3
    
  - 젓가락이  없으면  기다리는 Condition Variable C1, C2, C3
    
   - Lock 과 Condition Variable은  앞의  숙제처럼  리눅스  커널의  세마포로  구현
    
  - R1,R2,R3는  앞의  숙제처럼  텍스트  화일(fileVar)로  구현
    
   
		Take_R1() {
        
	        L1.Acquire();
        
	        s=Load(R1);
        
	        // R1이 1이면  젓가락이  있고 0이면  젓가락이  없어서  기다려야  함
        
	        while (s==0) {
        
	        // print message: getpid()가 R1을  기다림
        
	        C1.wait(&L1);
	        
	        // print message: getpid()가 R1을  기다리다가  깨어남
        
	        }
        
	        Store(R1,0);
        
	        // print message: getpid()가 R1을  가져옴
        
	        L1.Release();
        
        }
        
        // Take_R2(), Take_R3()도  구현
        
        Put_R1() {
        
	        L1.Acquire();
	        
	        Store(R1,1);
	        
	        C1.signal();
	        
	        // print message: getpid()가 R1을  기다림
	        
	        L1.Release();
        
        }
        
        // Put_R2(), Put_R3()도  구현
        
        Phil_A() {
        
	        Take_R1();
	        
	        // print message: getpid()가  생각을  시작함
	        
	        think(); // sleep()으로  구현
	        
	        // print message: getpid()가  생각을  멈춤
	        
	        Take_R2();
	        
	        // print message: getpid()가  먹기  시작함
	        
	        eating() // sleep()으로  구현
	        
	        // print message: getpid()가  먹기를  멈춤
	        
	        Put_R1();
	        
	        Put_R2();
        
        }
        
        // Phil_B(), Phil_C()도  구현
        
        main() {
        
	        for(i=0;i<100;i++)
	        
	        Phil_A();
        
        }
        
        // Phil_A, Phil_B, Phil_C 는  별도의  프로세스로  실행되어야  함

2. Phil_A, Phil_B, Phil_C 를  별도의  프로세스로  실행하여  데드락이  걸리는  상황을  만들고  이  상황의  스냅  샷을  보이고  설명하시오.

-   Tracing Messages :  적절한  메세지를  출력하여야  한다.
-   예:wait하게  될  때, wait에서  깨어날  때, 젓가락을  얻었을  때, 젓가락을  내려놓을  때  등

3. Deadlock을  다음  두가지  방법으로  해결하시오.

-   Prevention(예방) -  Circular wait 조건을  깨는  방법으로
-   Avoiding(회피) - 강의노트 p21 참조

4. Deadlock이  걸리는  상태와  이  문제가  해결된  두가지  종류의  상태를  보여주는  실행  결과를  제시하고  설명하시오.

-   Deadlock이  발생할  뻔한 부분(Tracing Message의 일부)을 보여주고 이 부분이 어떻게 에방되었는지 또는 어떻게 회피했는지를 설명


