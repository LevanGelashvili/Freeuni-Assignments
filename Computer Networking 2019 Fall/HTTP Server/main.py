import sys
import magic
import shutil
import os
import platform
import json
import socket
import threading
import calendar
from datetime import datetime
from datetime import date
from collections import OrderedDict

'''

ლოგები ტესტებზე ვერ გავატარე და მაგათ გარეშე დავწერე დავალება, ვერ ვხვდები რატომ არ გაიარა
ლოგის ჩაწერის და დირექტორიის/ფაილების გაკეთება/ჩაწერის კოდი მიწერია მაგრამ დაკომენტარებული მაქვს
მაგრამ ძველი ლოგები მაინც დატოვებული მაქვს, ჩახედვა ან რამის უკეთ გარჩევა თუ დაგჭირდათ


'''

# Writes a log into log directory
def write_log(line, headers, response, addr, code) :
    host = (headers['host'].split(':')[0] if ':' in headers['host'] else headers['host'])
    s = '[{}] '.format(response['date'])
    s += addr[0] + ' '
    s += host + ' '
    s += line[1] + ' '
    s += str(code) + ' '
    s += ('0' if 'content-length' not in response else response['content-length']) + ' '
    s += headers['user-agent'] + '\n'

    if code == 404 : host = 'error'
    with open(os.getcwd() + '/' + log + '/' + host + '.log',"a+") as f :
        f.write(s)
        f.close()


# Empties a directory and leaves one created
def create_empty_dir(path) :
    if os.path.isdir(path) : shutil.rmtree(path)
    os.mkdir(path)


# Maps (ip,port) to server info
def parse_config() :

    # Global variables, multiple threads will be using them, no need to pass them around
    global log
    global ip_map
    global host_set

    # Open passed configuration file, treating it as a dictionary
    with open(sys.argv[1],'r') as json_file :
        
        data = json.load(json_file)
        log = data['log'] # Path where logs are saved
        #create_empty_dir(os.getcwd() + '/' + log)
        servers = data['server'] # List of servers
        ip_map, host_set = dict(), set()

       # Iterates over servers to create a dict of unique pairs
        for server in servers :
            host_set.add(server['vhost'])
            pair = (server['ip'], server['port'])
            if pair not in ip_map : ip_map[pair] = []
            ip_map[pair].append(server)

        # Create log files for all hosts
        #for host in host_set : 
        #    f = open(os.getcwd() + '/' + log + '/' + host + '.log',"w+")
        #    f.close()
        #f = open(os.getcwd() + '/' + log + '/' + 'error' + '.log',"w+")
        #f.close()


# Search for file in directory, return None if not found or error
def search_for_file(file_path,rang) :
    try :
        f = open(file_path, mode = 'rb') # rb means reading binary data
        if rang == None : data = f.read()
        else : 
            left, right = rang[0], rang[1]
            f.seek(left, 0)
            if right == -1 : data = f.read()
            else : data = f.read(right - left + 1)
        return data
    except :
        return None


# Generates basic headers for response, needed for all requests
def generate_basic_headers(headers) :
    response = OrderedDict()
    response['server'] = '{} {}'.format(platform.system(), platform.release())
    response['date'] = '{}'.format(calendar.day_name[date.today().weekday()] + ', ' + datetime.now().strftime("%d/%m/%Y %H:%M:%S"))
    response['connection'] = ('keep-alive' if headers['connection'] == 'keep-alive' else 'close')
    if headers['connection'] == 'keep-alive' : response['keep-alive'] = 'timeout=5, max=999'
    response['accept-Ranges'] = 'bytes'
    response['etag'] = 'mudamtqveny'
    return response


# Build an http response string to send to client socket, returns string version
def build_response_string(response, head) :
    res = head
    for k, v in response.items() : res += k + ': ' + v + '\r\n'
    return res + '\r\n'


# Checks if virtual servers are hosting requested file
def file_on_server(pair, headers) :
    v_servers = ip_map[pair]
    for host in v_servers :
        if host['documentroot'] == headers['host'].split(':')[0] : return True
    return False


# Checks various input errors
def incorrect_input(headers) :
    host = headers['host'].split(':')
    return ('127.0.0.1', int(host[1])) not in ip_map or host[0] not in host_set

# Parses range string and returns a tuple of intervals(-1 denoting maximum size)
def parse_range(range_s) :
    return tuple(map(lambda x : (-1 if x == '' else int(x)), range_s.split('-')))

# Serves a client socket
def serve(pair, server, client_sock, addr) :

    client_sock.settimeout(5)

    while True :

        req = client_sock.recv(10000).decode()
        if not req : break
        # line is a list [Get/head, file path, http version]
        # headers is a dict of key value pairs in request headers
        line, headers = req.split('\n')[0].split(' '), {h[0].lower() : h[1][:-1] for h in list(map(lambda x : x.split(': '),req.split('\n')[1:]))[:-2]}

        path = headers['host'].split(':')[0] + line[1]
        path = path.replace('%20',' ') # ამის ბაგის გამო მაკსიდან მართვაზე გადასვლა მომინდა დედა მომიკვდეს

        response_map = generate_basic_headers(headers)

        # If virtual host doesn't have requested file, we send an error
        if not file_on_server(pair, headers) or incorrect_input(headers):
            s = build_response_string(response_map,'HTTP/1.1 404 NOT FOUND\r\n')
            #write_log(line, headers, response_map, addr, 404)
            client_sock.sendall(s.encode() + '\r\nREQUESTED DOMAIN NOT FOUND'.encode())
            client_sock.close()
            break

        
        rang = None
        if 'range' in headers : 
            response_map['range'] = headers['range']
            rang = parse_range(response_map['range'].split('=')[-1])
        requested_file = search_for_file(path, rang)
        
        # else we send 200 and add requested data if GET was received
        response_map['content-type'] = magic.from_file(path, True)
        response_map['content-length'] = (str(os.stat(path).st_size))

        response_str = build_response_string(response_map, 'HTTP/1.1 200 OK\r\n')
        #write_log(line, headers, response_map, addr, 200)

        if line[0] == 'GET' : client_sock.sendall(response_str.encode() + requested_file)
        else : client_sock.sendall(response_str.encode())
        client_sock.close()
        break

# Seperate thread for a server, pair denotes a tuple (ip, port)
def virtual_server(pair) :

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s :
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind(pair)
        s.listen()

        while True :
            client_sock, addr = s.accept()
            thread = threading.Thread(target = serve, args = (pair,ip_map[pair], client_sock, addr,))
            thread.start()


parse_config()
for pair in ip_map.keys() :
    thread = threading.Thread(target = virtual_server, args = (pair,))
    thread.start()