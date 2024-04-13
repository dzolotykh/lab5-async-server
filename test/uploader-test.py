import unittest
import struct
import socket
import json

class TestFileUpload(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cfg_file = open('config.json')
        cls.cfg = json.loads(cfg_file.read())
        cfg_file.close()
        cls.PORT = cls.cfg['server']['port']
        cls.HOST = '127.0.0.1'
        cls.sample = ("gm96qhkfyaltabk8csf03iz0r0t3cwhgbf6hmy6pohg04t5bqvzds096e75k3wk81c0uvabzk4njd63e4y5ywyfb7"
                      "e1thw8erjctomxq9xkrr0f376ebays0z63e3ysxidd9h3rkc19qeej354znlqexumu9c3errpelcvovq3c9yad6ci"
                      "h5gq9rews7jczuugopue1z")

    def test_small_file(self):
        s = self.sample
        serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        serversocket.connect((self.HOST, self.PORT))
        size = struct.pack('I', len(s))
        encoded_string = s.encode()
        serversocket.send("u".encode())
        serversocket.send(size)
        serversocket.send(encoded_string)
        size = serversocket.recv(4)
        size = int.from_bytes(size, byteorder='little')
        answer = serversocket.recv(size).decode()
        response = answer.split('|')
        status = response[0]
        self.assertEqual(status, 'OK', f'Server returned error: {response}')
        if status == 'OK':
            token = response[1]
            path = response[2]
            with open(path, 'rb') as f:
                in_file = f.read().decode()
                self.assertEqual(in_file, s, 'File content is not the same as the original string.')
        serversocket.close()

    def test_huge_file(self):
        s = self.sample * 20000
        serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        serversocket.connect((self.HOST, self.PORT))
        size = struct.pack('I', len(s))
        encoded_string = s.encode()
        serversocket.send("u".encode())
        serversocket.send(size)
        serversocket.send(encoded_string)
        size = serversocket.recv(4)
        size = int.from_bytes(size, byteorder='little')
        answer = serversocket.recv(size).decode()
        response = answer.split('|')
        status = response[0]
        self.assertEqual(status, 'OK', f'Server returned error: {response}')
        if status == 'OK':
            token = response[1]
            path = response[2]
            with open(path, 'rb') as f:
                in_file = f.read().decode()
                self.assertEqual(in_file, s, 'File content is not the same as the original string.')
        serversocket.close()

    def test_wrong_size_greater_than_original(self):
        s = self.sample
        serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        serversocket.connect((self.HOST, self.PORT))
        size = struct.pack('I', len(s) + 1)
        encoded_string = s.encode()
        serversocket.send("u".encode())
        serversocket.send(size)
        serversocket.send(encoded_string)
        serversocket.shutdown(socket.SHUT_WR)
        size = serversocket.recv(4)
        size = int.from_bytes(size, byteorder='little')
        answer = serversocket.recv(size).decode()
        response = answer.split('|')
        status = response[0]
        serversocket.close()
        self.assertEqual(status, 'ERROR', f'Server returned error: {response}')

class TestFileDownload(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cfg_file = open('config.json')
        cls.cfg = json.loads(cfg_file.read())
        cfg_file.close()
        cls.PORT = cls.cfg['server']['port']
        cls.HOST = '127.0.0.1'
        cls.sample = ("gm96qhkfyaltabk8csf03iz0r0t3cwhgbf6hmy6pohg04t5bqvzds096e75k3wk81c0uvabzk4njd63e4y5ywyfb7"
                      "e1thw8erjctomxq9xkrr0f376ebays0z63e3ysxidd9h3rkc19qeej354znlqexumu9c3errpelcvovq3c9yad6ci"
                      "h5gq9rews7jczuugopue1z")

if __name__ == '__main__':
    unittest.main()