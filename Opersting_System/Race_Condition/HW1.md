HW1: Race Condition
1. 다음 기능을 하는 프로그램(sum)을 만드시오
데이타 화일의 마지막 숫자를 읽어서 1을 더해서 그 화일의 끝에 추가해서 기록
데이타 화일은 초기에 0만 써있다.
위 함수를 1000번 호출
이 프로그램을 실행하면 0 부터 1000까지 1001개의 숫자가 기록될 것이다.
          char *CounterVar = "MyCounter";

int Load(char *fileVar) {
  // open file(fileVar)
  // read the last data
  // close file
}

void Store(char *fileVar, int data) {
  // open file(fileVar)
  // append the data
  // close file
}

int Add(int i,j) {
   return i+j
}

// 데이타 화일의 open, close는 Load, Store 각 함수 안에서 매번 수행하게할 것
main() {
   for ( ... ) { // 1000번?, 10000번?
      x=Load(fileVar);
      x=Add(x,1);
      Store(fileVar, x)
      }
}


2. 이 프로그램을 두개 이상 동시에 실행하여 결과를 살펴보시오
1) n개를 동시에 실행했을 때 데이타 화일의 마지막 숫자가 1000*n인가?
2) 결과가 1000*n이 아니라면 그 원인은 무엇인지 설명하시오. (데이타 화일 중 문제가 발생한 부분을 10줄 이내로 삽입)
3) 소스코드는 PDF 화일로 만들어 첨부하시오.

리눅스 쉘에서 프로그램을 두개 이상 동시에 실행하는 방법
$ ./sum & ./sum & ./sum        // 2개는 백그라운드에서 나머지 하나는 포그라운드에서 실행됨
// 이 경우 화일에 0을 포함해서 3001개의 숫자가 기록되어야 정상임