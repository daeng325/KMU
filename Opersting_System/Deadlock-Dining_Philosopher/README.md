
## HW3: Deadlock - Dining Philosopher

1. Dining Philosopher ������  ��������  �̿��Ͽ�  �����Ͻÿ�.

-   ÷�� ȭ�� ����

    
   - ö����  ���� A,B,C
    
   - ������  ���� R1,R2,R3
    
   - ��  �������� Mutual Exclusive �ϰ�  �����  ���� Lock L1, L2, L3
    
  - ��������  ������  ��ٸ��� Condition Variable C1, C2, C3
    
   - Lock �� Condition Variable��  ����  ����ó��  ������  Ŀ����  ��������  ����
    
  - R1,R2,R3��  ����  ����ó��  �ؽ�Ʈ  ȭ��(fileVar)��  ����
    
   
		Take_R1() {
        
	        L1.Acquire();
        
	        s=Load(R1);
        
	        // R1�� 1�̸�  ��������  �ְ� 0�̸�  ��������  ���  ��ٷ���  ��
        
	        while (s==0) {
        
	        // print message: getpid()�� R1��  ��ٸ�
        
	        C1.wait(&L1);
	        
	        // print message: getpid()�� R1��  ��ٸ��ٰ�  ���
        
	        }
        
	        Store(R1,0);
        
	        // print message: getpid()�� R1��  ������
        
	        L1.Release();
        
        }
        
        // Take_R2(), Take_R3()��  ����
        
        Put_R1() {
        
	        L1.Acquire();
	        
	        Store(R1,1);
	        
	        C1.signal();
	        
	        // print message: getpid()�� R1��  ��ٸ�
	        
	        L1.Release();
        
        }
        
        // Put_R2(), Put_R3()��  ����
        
        Phil_A() {
        
	        Take_R1();
	        
	        // print message: getpid()��  ������  ������
	        
	        think(); // sleep()����  ����
	        
	        // print message: getpid()��  ������  ����
	        
	        Take_R2();
	        
	        // print message: getpid()��  �Ա�  ������
	        
	        eating() // sleep()����  ����
	        
	        // print message: getpid()��  �Ա⸦  ����
	        
	        Put_R1();
	        
	        Put_R2();
        
        }
        
        // Phil_B(), Phil_C()��  ����
        
        main() {
        
	        for(i=0;i<100;i++)
	        
	        Phil_A();
        
        }
        
        // Phil_A, Phil_B, Phil_C ��  ������  ���μ�����  ����Ǿ��  ��

2. Phil_A, Phil_B, Phil_C ��  ������  ���μ�����  �����Ͽ�  �������  �ɸ���  ��Ȳ��  �����  ��  ��Ȳ��  ����  ����  ���̰�  �����Ͻÿ�.

-   Tracing Messages :  ������  �޼�����  ����Ͽ���  �Ѵ�.
-   ��:wait�ϰ�  ��  ��, wait����  ���  ��, ��������  �����  ��, ��������  ��������  ��  ��

3. Deadlock��  ����  �ΰ���  �������  �ذ��Ͻÿ�.

-   Prevention(����) -  Circular wait ������  ����  �������
-   Avoiding(ȸ��) - ���ǳ�Ʈ p21 ����

4. Deadlock��  �ɸ���  ���¿�  ��  ������  �ذ��  �ΰ���  ������  ���¸�  �����ִ�  ����  �����  �����ϰ�  �����Ͻÿ�.

-   Deadlock��  �߻���  ���� �κ�(Tracing Message�� �Ϻ�)�� �����ְ� �� �κ��� ��� ����Ǿ����� �Ǵ� ��� ȸ���ߴ����� ����


