import struct
import socket

PORT = 8080
HOST = '127.0.0.1'
sample = ("gm96qhkfyaltabk8csf03iz0r0t3cwhgbf6hmy6pohg04t5bqvzds096e75k3wk81c0uvabzk4njd63e4y5ywyfb7"
          "e1thw8erjctomxq9xkrr0f376ebays0z63e3ysxidd9h3rkc19qeej354znlqexumu9c3errpelcvovq3c9yad6ci"
          "h5gq9rews7jczuugopue1z")


def small_file():
    s = sample
    test_name = 'Small file test'
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.connect((HOST, PORT))
    size = struct.pack('I', len(s))
    encoded_string = s.encode()
    serversocket.send("u".encode())
    serversocket.send(size)
    serversocket.send(encoded_string)
    answer = serversocket.recv(1024).decode()
    print(answer)
    response = answer.split('@')
    status = response[0]
    if status != 'OK':
        print('❌ Test failed [', test_name, ']. Server returned error: ', response)
    else:
        token = response[1]
        path = response[2]
        with open(path, 'rb') as f:
            in_file = f.read().decode()
            if in_file == s:
                print('✅ Test passed [', test_name, ']')
            else:
                print('❌ Test failed [', test_name, ']. File content is not the same as the original string.')
    serversocket.close()


def huge_file():
    test_name = 'Huge file test'
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.connect((HOST, PORT))
    s = sample * 20000
    size = struct.pack('I', len(s))
    encoded_string = s.encode()
    serversocket.send("u".encode())
    serversocket.send(size)
    serversocket.send(encoded_string)
    answer = serversocket.recv(1024).decode()
    response = answer.split('@')
    status = response[0]
    if status != 'OK':
        print('❌ Test failed [', test_name, ']. Server returned error: ', response)
    else:
        token = response[1]
        path = response[2]
        with open(path, 'rb') as f:
            in_file = f.read().decode()
            if in_file == s:
                print('✅ Test passed [', test_name, ']')
            else:
                print('❌ Test failed [', test_name, ']. File content is not the same as the original string.')
    serversocket.close()


small_file()
huge_file()
