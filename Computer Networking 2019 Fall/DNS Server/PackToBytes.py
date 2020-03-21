import struct
import ipaddress
from ParserHelpers import types, byte_to_int

# Build response from scratch
def build_response(parser, data) :
    header_bytes = build_header(data, parser.header.info)
    question_bytes = build_question(parser.question.info)
    answer_bytes = build_answer(parser.answer.info)
    return header_bytes + question_bytes + answer_bytes


# Builds header
def build_header(data, info) :
    flags = byte_to_int(data[2:4]) # Flag to bytes
    # Make a header for default response, with 1 answers and 0 additional / authorities
    return struct.pack('!6H', *(info['ID'], flags, 1, 1, 0, 0))


# Build question
def build_question(info) :
    return domain_bytes(info['QNAME']) + struct.pack('!2H', *(info['QTYPE'], info['QCLASS']))


# Build answer
def build_answer(info) :

    rec_type = get_record_num(info['TYPE'])
    if rec_type is None : return b''

    data_bytes, length = get_data_bytes(info['RDDATA'], info['TYPE'])
    answer_bytes = domain_bytes(info['NAME'])
    answer_bytes += struct.pack('!H H I H', *(rec_type, info['CLASS'], info['TTL'], length))
    answer_bytes += data_bytes
    return answer_bytes


# Get key from dict with value
def get_record_num(type) :
    for k, v in types.items() : 
        if type == v : return k


# Returns byte representation of domain
def domain_bytes(domain) :
    # Since MX record is saved as a (preference, domain), we need to extract the string out of it
    if isinstance(domain, tuple) : domain = domain[1]
    return b''.join([bytes([len(s)]) + s.encode() for s in domain.split('.')])


# Treats each data type individually, transforms them to bytes
# Won't explain them in details, taken from rfc
def get_data_bytes(data, type) :

    # Since i cache values in lists, we parse data until it's not a list anymore
    while isinstance(data, list) : data = data[0]

    if type == 'A' : 
        if data.endswith('.') : data = data[:-1]
        return ipaddress.IPv4Address(data).packed, 4

    elif type in ('NS', 'CNAME') :
        res = domain_bytes(data)
        return res, len(res)

    elif type == 'MX' :
        res = struct.pack('!H', data[0]) + domain_bytes(data[1])
        return res, len(res)

    elif type == 'TXT' :
        if data.startswith('"') and data.endswith('"') : data = data[1:-1]
        res = struct.pack('B', len(data)) + data.encode()
        return res, len(res)

    elif type == 'SOA' :
        soa_list = data.split(' ')
        mname, rname = domain_bytes(soa_list[0]), domain_bytes(soa_list[1])
        return mname + rname + struct.pack('!5I', *tuple(map(int,soa_list[2:]))), 20 + len(mname) + len(rname)

    elif type == 'AAAA' :
        return ipaddress.IPv6Address(data).packed, 16