import socket
import struct

def connect_to_server():
    PORT = 8081
    HOST = '127.0.0.1'
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.connect((HOST, PORT))
    return serversocket

max_size = 1024 * 1024 * 10
sample = ("1 2 3 4 5")
def _send_upload_request(s, socket):
    size = struct.pack('I', len(s))
    encoded_string = s.encode()
    socket.send("u".encode())
    socket.send(size)
    # проверяем, не отключился ли сервер:
    try:
        socket.send(encoded_string)
    except BrokenPipeError:
        pass
    print("Sent upload request")
    response = socket.recv(1024)
    response2 = socket.recv(1024)
    print(response, '@', response2.decode())

def test_upload():
    s = connect_to_server()
    _send_upload_request(sample * 1000, s)
    s.close()

test_upload()