# Student ID : 20153194
# Name : Yunjeong Oh
from socket import *
import sys

serverPort = int(sys.argv[1])

# TCP서버소켓
serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind(('', serverPort))
serverSocket.listen(10)

print("Student ID : 20153194")
print("Name : Yunjeong Oh")

while True:
    print('\n')
    connectionSocket, addr = serverSocket.accept()

    request_message = connectionSocket.recv(1024).decode()

    # for IndexError
    if len(request_message)<10:
        continue

    print('Connection : Host IP {}, Port {}, socket {}'.format(addr[0], addr[1], connectionSocket))  # socket 4?
    print(request_message)  # print request message

    try:
        # get file name in request_message and read that file
        file_name = request_message.split()[1]
        file = open('.{}'.format(file_name),'r')
        data = file.read()
        file.close()
        # 해당하는 파일을 찾아서 HTTP Response 메시지로 만들어서 브라우저로 보낸다.
        header = 'HTTP/1.0 200 OK \r\nConnection: close \r\nID: 20153194 \r\nName: Yunjeong Oh \r\nContent-Length: {} \r\nContent-Type: text / html\r\n\r\n'.format(len(data))
        response_message = header + data

        sended_data_length = connectionSocket.send(response_message.encode())

        # 실제 전송한 바이트 수와 파일의 바이트 수를 출력
        print('finish ', sended_data_length - len(header), len(data))

    except Exception as ex:
        print('Server Error : No such file .{}!'.format(file_name))
        response_message = 'HTTP/1.0 404 NOT FOUND \r\nConnection: close \r\nID: 20153194 \r\nName: Yunjeong Oh \r\nContent-Length: 0 \r\nContent-Type: text/html\r\n\r\n'
        connectionSocket.send(response_message.encode())

    connectionSocket.close()

serverSocket.close()