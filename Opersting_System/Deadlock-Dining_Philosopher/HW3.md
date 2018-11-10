

# Deadlock

![
](https://raw.githubusercontent.com/daeng325/KMU/master/Opersting_System/Deadlock-Dining_Philosopher/images/os-hw3-deadlock.png)

Deadlock을 만들려면 4가지 조건이 필요하다. Mutual exclusive해야 하고, hold and wait가 있어야 하며, non-preemption (뺏거나 빼앗기지 않는다)이고, cycle이 있어야 한다. Mutual exclusive해야한다는 조건은 lock으로 해결했고, hold & wait조건은 3888을 A, 3889를 B, 3890을 C 프로세서라고 한다면, 실행결과로 인해 B가 R2를 잡고 R3를 기다리고, C가 R3를 잡고 R1을 기다리고, A가 R1을 잡고 R2를 기다리고 있는 걸 알 수 있다. 그리고 cycle이 있고, non-preemption이다. 그러므로 deadlock이 발생한다.

# Prevention

![enter image description here](https://raw.githubusercontent.com/daeng325/KMU/master/Opersting_System/Deadlock-Dining_Philosopher/images/os-hw3-prevention.png)

Deadlock을 막는 방법 중 circular wait (cycle)을 깨는 방법이 있는데, 그 방법은 모든 자원에 global한 순서를 매기는 방법이다. 보통은 낮은 번호를 먼저 잡고 높은 번호를 나중에 잡는다면, 마지막에만 높은번호->낮은번호 순으로 젓가락을 잡게 하면 cycle을 깰 수 있을 것이다. 그런데 위에 표시된 것처럼 process 5576이 R2->R3를 가지고 식사를 마친 후에 내려놓으려고 하는 와중에 process 5575가 내려놓기도 전에 R2를 가져가는 것을 볼 수 있다. 그 부분은 해결하지 못한 부분이다.

# Avoiding

![
](https://raw.githubusercontent.com/daeng325/KMU/master/Opersting_System/Deadlock-Dining_Philosopher/images/os-hw3-avoiding.png)


Deadlock을 피하는 방법 중 Banker’s algorithm 방법이 있다. 간단히 말하자면 safe한 상태이면 자원을 주는 개념이다. 이번 과제에서의 safe한 상태는 2가지 경우가 있다. 첫번째는 젓가락이 마지막 젓가락이 아닐 때이다. 두번째는 젓가락이 만약 1개가 남았으면, 누군가에게 줬을 때 젓가락이 2개가 되는 상태이다. 그 2가지 방향으로 가면 safe한 상태를 유지할 수 있을 것이다.

그리고 주의할 점은 avoiding을 하려면 lock을 하나로 통일해서 만들어야 한다는 것이었다. Take_R1()함수를 할 때도 R2나 R3상태를 확인해야 되기 때문에 경쟁조건이 발생할 것이다. 그리고 A,B,C 철학자가 몇 개 젓가락을 가지고 있는지 확인할 때도 경쟁조건이 발생할 것이다. 그래서 3개의 take함수, 3개의 put함수 모두 한 개의 lock으로 동기화해야 한다.
