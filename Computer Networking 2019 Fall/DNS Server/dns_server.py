import sys
import socket
import ipaddress
from Cache import Cache
from DnsParser import DnsParser
from ParserHelpers import root_servers, search_for_ip, types, log
from PackToBytes import build_response

# Recursively searches for requested info
def recursive_search(request, ip, port, cache) :

    client_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    client_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    client_sock.sendto(request, (ip, port))
    response, _ = client_sock.recvfrom(4096)
    client_sock.close()

    parser = DnsParser(response) # Parse response
    # These tuples represent (name, type) one for response and one for inital question
    resp_tuple = (parser.answer.info['NAME'], parser.answer.info['TYPE'])
    quest_tuple = (parser.question.info['QNAME'], types[parser.question.info['QTYPE']])
    
    log(parser) # Logs data

    # If this response answers our question
    if quest_tuple in cache.cache :
        # Pass our data to info
        parser.answer.info['RDDATA'] = cache.cache[quest_tuple]
        return build_response(parser, response)

    # If our response is valid (In those 7 types)
    if len(parser.answer.info) > 2 : cache.checkAndAdd(resp_tuple, parser.answer.info['RDDATA'])

    # If found the final answer, return in immediately
    if resp_tuple[1] in ('A', 'AAAA', 'TXT', 'MX', 'SOA') :
        return response

    # Else, redirect to the received domain
    elif resp_tuple[1] in ('NS', 'CNAME') :
        return recursive_search(request, search_for_ip(parser, parser.answer.info['RDDATA']), port, cache)


# Runs the dns server on passed ip and port
def run_dns_server(CONFIG, IP, PORT):

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((IP, int(PORT)))
    cache = Cache(CONFIG)

    while True :
        data, addr = server_socket.recvfrom(4096)
        data = recursive_search(data, root_servers[0], 53, cache)
        server_socket.sendto(data, addr)

    server_socket.close()


# do not change!
if __name__ == '__main__':
    CONFIG = sys.argv[1]
    IP = sys.argv[2]
    PORT = sys.argv[3]
    run_dns_server(CONFIG, IP, PORT)