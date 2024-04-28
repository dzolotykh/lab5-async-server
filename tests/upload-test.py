import socket
import struct

def connect_to_server():
    PORT = 8080
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
    response = socket.recv(1024).decode()
    with open('../uploads/' + response, 'r') as f:
        return response, f.read() == s

def _send_download_request(token, socket):
    encoded_string = token.encode()
    socket.send("d".encode())
    socket.send(encoded_string)
    size = int.from_bytes(socket.recv(4), 'little')
    print(size)
    file = socket.recv(size).decode()
    with open('../uploads/' + token, 'r') as f:
        return f.read() == file

def test_upload():
    s = connect_to_server()
    token, check_result = _send_upload_request(sample * 1000, s)
    if check_result:
        print(f'✅ Тест загрузки пройден успешно. Токен: {token}')
    else:
        print(f'❌ Тест загрузки не пройден. Токен: {token}')
    s.close()
    s = connect_to_server()
    result = _send_download_request(token, s)
    s.close()
    if result:
        print(f'✅ Тест скачивания пройден успешно. Токен: {token}')
    else:
        print(f'❌ Тест скачивания не пройден. Токен: {token}')



test_upload()