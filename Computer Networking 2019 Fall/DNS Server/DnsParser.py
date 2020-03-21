from ParserHelpers import list_of_binary_strings, byte_to_int, domain_name, types

# Parses whole dns record, saves its header, question, answer and a list of authorities and additionals
class DnsParser :

    def __init__(self, data) :

        self.header = HeaderParser(data) #Data pointer at 0, processes 12 bytes
        self.question = QuestionParser(data) #Data pointer at 12, processes who knows how many bytes, we pass whole data for offset purposes
        self.answer = RRParser(data, self.question.ptr)
        self.authorities, self.additionals = [], []
        self.cur_ptr = self.answer.cur_ptr

        # Inner function, not to repeat code, since authorities and additionals are repserented same way
        # Saving them in an exactly same manner would be reasonable
        def add_rr(arr, key) :
            for _ in range(self.header.info[key]) :
                arr.append(RRParser(data, self.cur_ptr))

                if len(arr[-1].info) == 2 : continue
                self.cur_ptr = arr[-1].cur_ptr

        add_rr(self.authorities, 'NSCOUNT')
        add_rr(self.additionals, 'ARCOUNT')


# Reads first 12 bytes of DNS message and processes its headers
class HeaderParser :

    def __init__(self, data) :

        self.info = {}

        # Turns headers into binary strings, 6 means that we parse 6 shorts (length of short = 2 (12/6)) from message
        bin_headers = list_of_binary_strings(data[:12], 6)
        self.info['ID'] = int(bin_headers[0], 2)

        flags = bin_headers[1] # Binary string of flags
        self.info['QR'] = int(flags[0], 2)
        self.info['OPCODE'] = int(flags[1:5], 2)
        self.info['AA'] = int(flags[5], 2)
        self.info['TC'] = int(flags[6], 2)
        self.info['RD'] = int(flags[7], 2)
        self.info['RA'] = int(flags[8], 2)
        self.info['Z'] = int(flags[9:12], 2)
        self.info['RCODE'] = int(flags[12:], 2)

        self.info['QDCOUNT'] = int(bin_headers[2], 2)
        self.info['ANCOUNT'] = int(bin_headers[3], 2)
        self.info['NSCOUNT'] = int(bin_headers[4], 2)
        self.info['ARCOUNT'] = int(bin_headers[5], 2)


# Reads Question part of the message and processes it
class QuestionParser :

    def __init__(self, data) :

        self.data = data
        self.info = {}

        q_name, bytes_read = domain_name(data, 12) # Returns number of currently read bytes too
        self.info['QNAME'] = q_name # Removes last '.' returned from recursive method
        self.info['QTYPE'] = byte_to_int(data[bytes_read : bytes_read + 2])
        self.info['QCLASS'] = byte_to_int(data[bytes_read + 2 : bytes_read + 4])

        self.ptr = bytes_read + 4 # Denotes where pointer is standing at this moment


# Reads the answer part and processes it
class RRParser :

    def __init__(self, data, ptr) :

        name, bytes_read = domain_name(data, ptr) # bytes_read should denote current pointer

        self.info = {}
        self.info['NAME'] = name
        cur_type = byte_to_int(data[bytes_read : bytes_read + 2])

        # If type is OPT (41) or some else type we don't need to handle, return blank
        if cur_type not in types:
            self.info['TYPE'] = 'UNDEFINED'
            self.cur_ptr = ptr
            return None

        self.info['TYPE'] = types[cur_type]
        self.info['CLASS'] = byte_to_int(data[bytes_read + 2 : bytes_read + 4])
        self.info['TTL'] = byte_to_int(data[bytes_read + 4 : bytes_read + 8])
        self.info['RDLENGTH'] = byte_to_int(data[bytes_read + 8 : bytes_read + 10])

        parsed_data, data_length = RRParser.parse_response_data(self.info['TYPE'], data, bytes_read + 10, self.info['RDLENGTH'])
        self.info['RDDATA'] = parsed_data
        self.cur_ptr = 10 + bytes_read + data_length


    @staticmethod
    # Parses response data based on its type
    # Returns tuple of data and its length (in bytes)
    # Taken from rfc, no need to explain in details
    def parse_response_data(type, data, ptr, rd_length) :

        import ipaddress

        if type == 'A' :
            return (ipaddress.IPv4Address(data[ptr : ptr + 4]).compressed, 4)

        elif type in ('NS', 'CNAME') :
            ans = domain_name(data, ptr)
            return ans[0], ans[1] - ptr

        elif type == 'MX' :
            preference = byte_to_int(data[ptr : ptr + 2])
            ans = domain_name(data, ptr + 2)
            return (preference, ans[0]), ans[1] - ptr
        
        elif type == 'TXT' :
            return (data[ptr : ptr + rd_length], rd_length)

        elif type == 'SOA' :

            mname, cur_ptr = domain_name(data, ptr)
            rname, cur_ptr = domain_name(data, ptr)
            serial = str(byte_to_int(data[cur_ptr : cur_ptr + 4]))
            refresh = str(byte_to_int(data[cur_ptr + 4 : cur_ptr + 8]))
            retry = str(byte_to_int(data[cur_ptr + 8 : cur_ptr + 12]))
            expire = str(byte_to_int(data[cur_ptr + 12: cur_ptr + 16]))
            minimum = str(byte_to_int(data[cur_ptr + 16: cur_ptr + 20]))

            return (' '.join([mname, rname, serial, refresh, retry, expire, minimum])[1:], cur_ptr + 20 - ptr)

        elif type == 'AAAA' :
            return (ipaddress.IPv6Address(data[ptr : ptr + 16]).compressed, 16)