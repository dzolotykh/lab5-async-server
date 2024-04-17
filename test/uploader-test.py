import os
import unittest
import struct
import socket
import json
import psycopg2
from dotenv import load_dotenv

def get_env_var(name):
    try:
        return os.environ[name]
    except KeyError:
        message = f"Expected environment variable {name} not set."
        raise Exception(message)
def connect_to_database():
    cfg_file = open('config.json')
    cfg = json.loads(cfg_file.read())
    cfg_file.close()
    PORT = get_env_var(cfg['database']['port'])
    HOST = get_env_var(cfg['database']['host'])
    NAME = get_env_var(cfg['database']['name'])
    USER = get_env_var(cfg['database']['user'])
    PASSWORD = get_env_var(cfg['database']['password'])
    conn = psycopg2.connect(
        host=HOST,
        database=NAME,
        user=USER,
        password=PASSWORD,
        port=PORT
    )
    return conn

def request_path(token, conn):
    cur = conn.cursor()
    cur.execute(f"SELECT filepath FROM files WHERE token = '{token}'")
    path = cur.fetchone()[0]
    cur.close()
    return path

def connect_to_server():
    cfg_file = open('config.json')
    cfg = json.loads(cfg_file.read())
    cfg_file.close()
    PORT = cfg['server']['port']
    HOST = '127.0.0.1'
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.connect((HOST, PORT))
    return serversocket


class TestFileUploader:
    def init(self):
        cfg_file = open('config.json')
        self.cfg = json.loads(cfg_file.read())
        cfg_file.close()
        self.max_size = self.cfg['file-uploader']['max_file_size']
        self.sample = ("gm96qhkfyaltabk8csf03iz0r0t3cwhgbf6hmy6pohg04t5bqvzds096e75k3wk81c0uvabzk4njd63e4y5ywyfb7"
                      "e1thw8erjctomxq9xkrr0f376ebays0z63e3ysxidd9h3rkc19qeej354znlqexumu9c3errpelcvovq3c9yad6ci"
                      "h5gq9rews7jczuugopue1z")


    def _send_upload_request(self, s, socket):
        size = struct.pack('I', len(s))
        encoded_string = s.encode()
        socket.send("u".encode())
        socket.send(size)
        # проверяем, не отключился ли сервер:
        try:
            socket.send(encoded_string)
        except BrokenPipeError:
            pass

    def _send_upload_request_with_size(self, s, s_size, socket):
        size = struct.pack('I', s_size)
        encoded_string = s.encode()
        socket.send("u".encode())
        socket.send(size)
        socket.send(encoded_string)

    def _recive_response(self, socket):
        size = socket.recv(4)
        size = int.from_bytes(size, byteorder='little')
        answer = socket.recv(size).decode()
        response = answer.split('|')
        return response[0], response[1:]

    def test_small_file(self):
        test_name = "test_small_file"
        print(f"🍀Running test {test_name}...")
        self.init()
        s = self.sample
        serversocket = connect_to_server()
        self._send_upload_request(s, serversocket)
        status, payload = self._recive_response(serversocket)

        if status != 'OK':
            serversocket.close()
            return f'❌ Тест с именем {test_name} не пройден: вместо ОК сервер вернул {status} + {payload}'
        if status == 'OK':
            conn = connect_to_database()
            token = payload[0]
            path = request_path(token, conn)
            conn.close()
        with open(path, 'rb') as f:
                in_file = f.read().decode()
        serversocket.close()

        if in_file != s:
            return f'❌ Тест с именем {test_name} не пройден: содержимое файла не совпадает с отправленным'
        return f'✅ Тест с именем {test_name} пройден'

    def test_wrong_size_greater_than_original(self):
        test_name = 'test_wrong_size_greater_than_original'
        print(f"🍀Running test {test_name}...")
        self.init()
        s = self.sample
        serversocket = connect_to_server()
        self._send_upload_request_with_size(s, len(s) + 1, serversocket)
        serversocket.shutdown(socket.SHUT_WR)
        status, response = self._recive_response(serversocket)
        serversocket.close()
        if status != 'ERROR':
            return f'❌ Тест {test_name} не пройден: вместо ERROR сервер вернул {status} + {response}'
        return f'✅ Тест {test_name} пройден'

    def test_max_file_size(self):
        test_name = 'test_max_file_size'
        print(f"🍀Running test {test_name}...")
        self.init()
        s = 'x' * self.max_size
        serversocket = connect_to_server()
        self._send_upload_request(s, serversocket)
        serversocket.shutdown(socket.SHUT_WR)
        status, response = self._recive_response(serversocket)
        serversocket.close()
        if status != 'OK':
            return f'❌ Тест {test_name} не пройден: вместо OK сервер вернул {status} + {response}'
        if status == 'OK':
            conn = connect_to_database()
            token = response[0]
            path = request_path(token, conn)
            conn.close()
            with open(path, 'rb') as f:
                in_file = f.read().decode()
            if in_file != s:
                return f'❌ Тест {test_name} не пройден: содержимое файла не совпадает с отправленным'
        return f'✅ Тест {test_name} пройден'

    def test_greater_than_max_file_size(self):
        self.init()
        test_name = 'test_greater_than_max_file_size'
        print(f"🍀Running test {test_name}...")
        s = 'x' * (self.max_size + 1)
        serversocket = connect_to_server()
        self._send_upload_request(s, serversocket)
        try:
            serversocket.shutdown(socket.SHUT_WR)
        except:
            pass
        status, response = self._recive_response(serversocket)
        serversocket.close()
        if status != 'ERROR':
            return f'❌ Тест {test_name} не пройден: вместо ERROR сервер вернул {status} + {response}'
        return f'✅ Тест {test_name} пройден'


if __name__ == '__main__':
    load_dotenv()
    test = TestFileUploader()
    print(test.test_small_file())
    print(test.test_wrong_size_greater_than_original())
    print(test.test_max_file_size())
    print(test.test_greater_than_max_file_size())