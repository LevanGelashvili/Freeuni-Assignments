import struct
import socket

# Denotes values of numbers to be inserted in struct packing method, 1 is for chars, 2 for shorts, 4 for ints
unsigned_number_bytes = {1:'B', 2:'H', 4:'I'}

# Receives two arguments, data is array of bytes to convert to binary strings
# num_strings denotes amount of strings returned
def list_of_binary_strings(data, num_strings) :
    return list(map(lambda s : bin(s)[2:].zfill(8 * (len(data) // num_strings)), struct.Struct('!{}{}'.format(num_strings, unsigned_number_bytes[len(data) // num_strings])).unpack(data)))


# Receives one argument, array of bytes, turns them into binary strings, concatenates and returns its decimal value
def byte_to_int(byte_arr) :
    return int(''.join(map(lambda s : bin(s)[2:], struct.Struct('!{}'.format(unsigned_number_bytes[len(byte_arr)])).unpack(byte_arr))), 2)


# Takes two arguments : data and a pointer which is pointing at specific index
# Returns domain name parsed from the message and offset from beginning, where pointer is currently standing
def domain_name(data, ptr) :

    char_count = byte_to_int(data[ptr : ptr + 1]) # Takes one char from data

    # If an 8 bit number if >= 192, then its first two bits are '11', which means that it's a pointer in DNS comperssion
    if char_count >= 192 :
        
        # Extract last 14 bits
        ans = domain_name(data, int(bin(struct.Struct('!H').unpack(data[ptr : ptr + 2])[0])[4:], 2))
        return ans[0], ptr + 2

    else :
        
        if char_count == 0 : return '', ptr + 1 # We return gathered string, if end was reached
        cur_str = ''.join(map(chr, data[ptr + 1 : ptr + 1 + char_count]))
        
        # Method is recursive, first gather everything before first '.', then append everything else
        tuple_ans = domain_name(data, ptr + 1 + char_count)
        return cur_str + '.' + tuple_ans[0], tuple_ans[1]


# Searches for IP in records, returns a tuple (True, ip) if found one
# Or (False, next domain name) if has to search from the root again
# Returns None if didn't find anything
def search_for_ip(parser, domain) :

    # Concatenate all RR records together
    infos = [parser.answer] + parser.authorities + parser.additionals

    # Inner boolean method, not to repeat code
    def found_info(info, key, type_val) :
        return len(info) > 2 and key in info and info[key] == domain and info['TYPE'] in type_val

    # If found exact ip return it
    for query in infos :
        if found_info(query.info, 'NAME', ('A')) :
            return query.info['RDDATA']
    
    # If found next domain (like in twitch.tv example) return it
    for query in infos :
        if found_info(query.info, 'RDDATA', ('NS', 'CNAME')) :
            return query.info['RDDATA']


# IP addresses of 13 root servers
root_servers = ['198.41.0.4', '192.228.79.201', '192.33.4.12', '199.7.91.13', '192.203.230.10', '192.5.5.241', 
                '192.112.36.4', '128.63.2.53', '192.36.148.17', '192.58.128.30', '193.0.14.129', '199.7.83.42', '202.12.27.33']

# Taken from RFC, mapping type int values to their str values
types = { 1: 'A', 2: 'NS', 15: 'MX', 16: 'TXT', 6: 'SOA', 28: 'AAAA', 5: 'CNAME'}

# Logs (prints in terminal) received response information
def log(parser) :
    if parser.answer.info['TYPE'] != 'UNDEFINED' :
        print('LOG :      {}     {}     {}     {}     {}'.format(parser.answer.info['NAME'].ljust(15, ' '), 
                str(parser.header.info['ID']).ljust(6, ' '), 'IN', parser.answer.info['TYPE'].ljust(6, ' '), parser.answer.info['RDDATA']))