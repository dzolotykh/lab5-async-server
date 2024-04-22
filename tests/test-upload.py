import socket
import struct

def connect_to_server():
    PORT = 8081
    HOST = '127.0.0.1'
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.connect((HOST, PORT))
    return serversocket

max_size = 1024 * 1024 * 10
sample = ("gm96qhkfyaltabk8csf03iz0r0t3cwhgbf6hmy6pohg04t5bqvzds096e75k3wk81c0uvabzk4njd63e4y5ywyfb7"
                   "e1thw8erjctomxq9xkrr0f376ebays0z63e3ysxidd9h3rkc19qeej354znlqexumu9c3errpelcvovq3c9yad6ci"
                   "h5gq9rews7jczuugopue1z")
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
    print(response.decode())

def test_upload():
    s = connect_to_server()
    _send_upload_request(sample * 1000, s)
    s.close()

test_upload()
