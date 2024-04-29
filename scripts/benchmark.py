import asyncio
import struct
from random import randint, shuffle
import time
import subprocess


async def connect_to_server():
    PORT = 8080
    HOST = '127.0.0.1'
    reader, writer = await asyncio.open_connection(HOST, PORT)
    return reader, writer


async def read_graph() -> list[list[int]]:
    with open('graph.txt', 'r') as f:
        return [list(map(int, i.split())) for i in str(f.read()).split('\n')]


async def generate_upload_request_string():
    graph = await read_graph()
    v, e = graph[0]
    graph = graph[1:]
    shuffle(graph)
    graph_s = f'{v} {e}\n' + '\n'.join([' '.join(map(str, i)) for i in graph])
    size = struct.pack('I', len(graph_s))
    encoded_string = graph_s.encode()
    request_string = "u".encode() + size + encoded_string
    return request_string


async def send_upload_request(request_string: bytes):
    reader, writer = await connect_to_server()
    writer.write(request_string)
    await writer.drain()
    response = await reader.read(1024)
    writer.close()
    return response.decode()


async def send_generation_request(request_string: bytes):
    reader, writer = await connect_to_server()
    writer.write(request_string)
    await writer.drain()
    response = await reader.read(1024)
    writer.close()
    return response.decode()


async def time_upload(times=100):
    total_time = 0
    for i in range(times):
        s = await generate_upload_request_string()
        start = time.time_ns()
        await send_upload_request(s)
        finish = time.time_ns()
        total_time += (finish - start) / 1_000_000
    print(total_time / times)
    return total_time / times


async def main(test_num=1):
    test_times = []
    for i in range(test_num):
        tasks = [time_upload() for _ in range(10)]
        times = await asyncio.gather(*tasks)
        test_times.append(sum(times) / len(times))
    print('Среднее время загрузки:', sum(test_times) / len(test_times), 'мс')


asyncio.run(main())
