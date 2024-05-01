import socket
import struct

def connect_to_server():
    PORT = 8080
    HOST = '127.0.0.1'
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.connect((HOST, PORT))
    return serversocket

def upload_data(input):
    socket = connect_to_server()
    with open(input) as f:
        s = str(f.read())
    encoded_input = s.encode()
    size = struct.pack('I', len(s))
    socket.send("u".encode())
    socket.send(size)
    socket.send(encoded_input)
    response = socket.recv(1024)
    socket.close()
    return response.decode()

def generate_request(token):
    socket = connect_to_server()
    socket.send(('g' + token).encode())
    response = socket.recv(1024)
    socket.close()
    return response.decode()

def save_file(to, token):
    socket = connect_to_server()
    socket.send('d'.encode())
    socket.send(token.encode())
    size = int.from_bytes(socket.recv(4), 'little')
    print(size)
    read_bytes = 0
    with open(to, 'wb') as f:
        while read_bytes < size:
            data = socket.recv(size)
            read_bytes += len(data)
            f.write(data)
    socket.close()

input_file = 'input/graph-1.txt'
token = upload_data(input_file)
generation_token = generate_request(token)
print(generation_token)
save_file('output/graph-1.bmp', generation_token)