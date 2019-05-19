# Student ID : 20153194
# Name : Yunjeong Oh
from socket import *
import sys
import select

serverPort = int(sys.argv[1])
MAXCLIENT = 16

# TCP서버소켓
serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind(('', serverPort))
serverSocket.listen(10)

# 관리하고 싶은 socket들.
clienttcpSocket = [serverSocket]

print("Student ID : 20153194")
print("Name : Yunjeong Oh")

while True:
    try:
        input_ready, write_ready, except_ready = select.select(clienttcpSocket, [], [])
        
        # 응답받은 read 리스트 처리
        for ir in input_ready:
            # 클라이언트가 접속했으면 처리함
            if ir == serverSocket :
                # MAXCLIENT 수보다 크면 connection 만들었다가 닫음. 
                if len(clienttcpSocket) > MAXCLIENT : 
                    connectionSocket,addr = serverSocket.accept()
                    connectionSocket.close()
                    continue
                # MAXCLIENT 수보다 작으면 다음을 수행
                connectionSocket, addr = serverSocket.accept()
                print('Connection from host {}, port {}, socket {}'.format(addr[0], addr[1], connectionSocket.fileno())) 
                # clienttcpSocket에 추가함으로써 데이터가 들어오는 것을 감시함
                clienttcpSocket.append(connectionSocket)
    
            # 클라이언트소켓에 데이터가 들어왔으면
            else:
                data = ir.recv(1024)
                # telnet 종료 시 '^]' 가 들어오면 connection 종료 & 리스트에서 제거.
                if data==b'^]\r\n' or data ==b'\xff\xf4\xff\xfd\x06':
                    print('Connection Closed {}'.format(ir.fileno()))
                    clienttcpSocket.remove(ir)
                    ir.close()
                # 받은 data(종료문자가 아닌)가 있으면 다른 client들한테 전송.
                elif data:
                    # print(ir.getpeername(), 'send :', data, flush=True)
                    for i in clienttcpSocket:
                        if i==ir or i==serverSocket:
                            continue
                        i.send(data)
                # 데이터가 없는경우, 즉 클라이언트에서 소켓을 close 한경우(창을 닫았을 때)
                else:
                    print('Connection Closed {}'.format(ir.fileno()))
                    # print(ir.getpeername(), 'close', flush=True)
                    ir.close()
                    # 리스트에서 제거
                    clienttcpSocket.remove(ir)

    except error as e:
        print ("Error creating socket: %s" % e)
        sys.exit(1)

serverSocket.close()