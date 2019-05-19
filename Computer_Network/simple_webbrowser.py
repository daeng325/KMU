# Student ID : 20153194
# Name : Yunjeong Oh
from socket import *

print("Student ID : 20153194")
print("Name : Yunjeong Oh")

while True:
    input_list = input('\n> ') # cmd
    if(input_list is None or input_list=="" ):
        continue
    if(input_list=='quit'):
        exit(0)

    if(input_list.split()[0]!='wget') :
        print('Wrong command {}'.format(input_list.split()[0]))
        continue

    hostName , portNum, fileName = input_list.split()[1], input_list.split()[2], input_list.split()[3]

    request_message = 'GET {} HTTP/1.0 \nHost: {} \nUser-agent: HW1/1.0 \nID: 20153194 \nName: Yunjeong Oh \nConnection: close\n\n' \
        .format(fileName,hostName)
    
    # create TCP socket for server
    clientSocket = socket(AF_INET, SOCK_STREAM)
    try:
        clientSocket.connect((hostName,int(portNum)))
        clientSocket.send(request_message.encode())
        buffer_size = 1024
        data = clientSocket.recv(buffer_size)

        print(data)

        split_data = data.split(b'\r\n\r\n')[1] # body
        
        status_code , status_txt = data.split()[1], data.split(b'\r\n')[0].split()[2:] # Http Response Status Code, Status text
        
        save_file_name = fileName.split('/')[-1]
        
        if(status_code!=b'200'): # print status_code & status_txt
            error_status = ''
            for s in status_txt:
                error_status = error_status + s.decode() + ' '
            print(int(status_code),error_status)
        else: # status is  200 OK
            total_size = int(data.split(b'Content-Length: ')[1].split(b'\r\n')[0])
            print('Total Size {} bytes'.format(total_size)) 

            size = len(split_data)
            count = 1
            while size<total_size :
                if(int(size/total_size*100)>10*count ):
                    print('Current Downloading {}/{} (bytes) \t {}%'.format(size,total_size,int(size/total_size*100)))
                    count += 1
                split_data += clientSocket.recv(buffer_size)
                size = len(split_data)
                
            f = open('./{}'.format(save_file_name),'wb')
            f.write(split_data)
            f.close()
            print('Current Downloading {}/{} (bytes) \t {}%'.format(size,total_size,100))
            print('Download Complete: {}, {}/{}'.format(save_file_name, size, total_size))

        # print('split_data : ', len(split_data))
        # print('size :{}, total_size :{}'.format(size, total_size))
    
    except Exception as ex:
        print('{} : unknown host'.format(hostName))
        print('cannot connect to server {} {}'.format(hostName,int(portNum)), ex) # ex 프린트하는 거 지워야 됨.

    clientSocket.close()
