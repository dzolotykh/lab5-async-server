import asyncio
import struct
from random import randint, shuffle
import time
import subprocess
import socket
import threading
import concurrent.futures


def connect_to_server():
    PORT = 8081
    HOST = '77.221.158.36'
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.connect((HOST, PORT))
    return serversocket


def read_graph() -> list[list[int]]:
    with open('graph.txt', 'r') as f:
        return [list(map(int, i.split())) for i in str(f.read()).split('\n')]


def generate_upload_request_string():
    graph = read_graph()
    v, e = graph[0]
    graph = graph[1:]
    shuffle(graph)
    graph_s = f'{v} {e}\n' + '\n'.join([' '.join(map(str, i)) for i in graph])
    size = struct.pack('I', len(graph_s))
    encoded_string = graph_s.encode()
    request_string = "u".encode() + size + encoded_string
    return request_string


def send_upload_request(request_string: bytes):
    socket = connect_to_server()
    sent_bytes = 0
    while sent_bytes < len(request_string):
        g = socket.send(request_string[sent_bytes:])
        sent_bytes += g
    response = socket.recv(1024)
    socket.close()
    return response.decode()


def send_generation_request(token: str) -> str:
    socket = connect_to_server()
    socket.send('g'.encode() + token.encode())
    response = socket.recv(32)
    socket.close()
    return response.decode()

def send_download_request(token: str):
    socket = connect_to_server()
    socket.send('d'.encode() + token.encode())
    size = int.from_bytes(socket.recv(4), 'little')
    file = b''
    while len(file) < size:
        file += socket.recv(size - len(file))
    socket.close()



def upload_generate_download_time():
    start = time.time_ns()
    token = send_upload_request(generate_upload_request_string())
    finish = time.time_ns()
    time_upload = (finish - start) / 1_000_000
    start = time.time_ns()
    token = send_generation_request(token)
    finish = time.time_ns()
    time_generate = (finish - start) / 1_000_000
    start = time.time_ns()
    send_download_request(token)
    finish = time.time_ns()
    time_download = (finish - start) / 1_000_000

    return time_upload, time_generate, time_download


def run_benchmark(threads, times_in_thread):
    time_upload = 0
    time_generate = 0
    time_download = 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=threads) as executor:
        futures = [executor.submit(upload_generate_download_time) for _ in range(times_in_thread * threads)]
        print('Всего потоков:', threads)
        print('Всего запусков в потоках:', times_in_thread)
        cnt = 0
        for future in concurrent.futures.as_completed(futures):
            time_upload_thread, time_generate_thread, time_download_thread = future.result()
            time_upload += time_upload_thread
            time_generate += time_generate_thread
            time_download += time_download_thread
            cnt += 1
            if cnt % 10 == 0:
                print(f'Пройдено {cnt} из {threads * times_in_thread} запусков')

    time_upload /= (threads * times_in_thread)
    time_generate /= (threads * times_in_thread)
    time_download /= (threads * times_in_thread)

    return time_upload, time_generate, time_download


if __name__ == '__main__':
    threads = 5
    times_in_thread = 100
    time_upload, time_generate, time_download = run_benchmark(threads, times_in_thread)
    print(f'Среднее время загрузки: {time_upload} мс')
    print(f'Среднее время генерации: {time_generate} мс')
    print(f'Среднее время скачивания: {time_download} мс')
