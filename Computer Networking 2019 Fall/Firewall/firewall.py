from main import PKT_DIR_INCOMING, PKT_DIR_OUTGOING
PROTOCOLS = {1: 'icmp', 6: 'tcp', 17: 'udp'}

class Firewall:
    def __init__(self, config, iface_int, iface_ext):
        self.iface_int = iface_int
        self.iface_ext = iface_ext
        self.rules = self.load_rules(config['rule'])
        self.geo = self.load_geo('geoipdb.txt')
        self.debug = True


    # @pkt_dir: either PKT_DIR_INCOMING or PKT_DIR_OUTGOING
    # @pkt: the actual data of the IPv4 packet (including IP header)
    def handle_packet(self, pkt_dir, pkt):
        pkt_copy = [format(s, 'b').zfill(8) for s in bytearray(pkt)]
        protocol, source_ip, dest_ip, data = self.parse_IP(pkt_copy)

        external_ip = source_ip if pkt_dir == PKT_DIR_INCOMING else dest_ip 
        external_port = self.parse_transport(protocol, data, pkt_dir)
        ip_country = self.binary_search_country(external_ip)

        if self.debug:
            print('----------------------START-----------------------------')
            print('Packet direction:', 'outgoing' if pkt_dir else 'incoming')
            print(protocol, source_ip, dest_ip, external_port, ip_country)
            print('----------------------RULES-----------------------------')

        matched_rules = [rule for rule in self.rules if rule[1] == protocol and self.port_matches(external_port, rule[3]) and self.ip_matches(external_ip, rule[2], ip_country)]

        if protocol == 'udp' and pkt_dir == PKT_DIR_OUTGOING and external_port == 53: 
            matched_rules += self.matched_dns_rules(data)

        if self.debug: 
            print('Matched rules: ', matched_rules)
        
        if not matched_rules or matched_rules[0][0] == 'pass' :
            self.send_packet(pkt_dir, pkt)
        
        if self.debug:
            print('-----------------------OVER------------------------------\n\n')


    # Returns country code of an ip
    def binary_search_country(self, ip):

        low, high = 0, len(self.geo) - 1

        while True:
            mid = (low + high) / 2
            low_ip, high_ip = self.geo[mid][0], self.geo[mid][1]

            if self.comp_ip(ip, low_ip) == -1:
                high = mid - 1
            elif self.comp_ip(ip, high_ip) < 1:
                return self.geo[mid][2].lower()
            else :
                low = mid + 1


    # Compares two ip addresses with general convention
    def comp_ip(self, ip1, ip2):
        ip1, ip2 = map(int, ip1.split('.')), map(int, ip2.split('.'))
        for i in range(len(ip1)):
            if ip1[i] > ip2[i]: return 1
            elif ip1[i] < ip2[i]: return -1
        return 0


    # Returns a list of matched dns rules, empty if packet doesn't match standarts
    def matched_dns_rules(self, data):
        offset = 8
        qdcount, offset = int(data[4 + offset] + data[5 + offset], 2), offset + 12
        
        domain, offset = self.parse_domain(data, offset)
        qtype, offset = int(data[offset] + data[offset + 1], 2), offset + 2
        qclass = int(data[offset] + data[offset + 1], 2)

        if self.debug:
            print('DNS parameters', domain, qdcount, qtype, qclass)

        if qdcount != 1 or qtype not in (1, 28) or qclass != 1: return []
        return [rule for rule in self.rules if rule[1] == 'dns' and self.domains_match(domain, rule[2])]


    # Returns whether two domains match each other
    def domains_match(self, my_domain, rule_domain):
        if '*' not in rule_domain: return my_domain == rule_domain
        return rule_domain == '*' or my_domain.endswith(rule_domain[1:])


    # Returns parsed domain and new offset
    def parse_domain(self, data, offset):
        domain = ''
        while True:
            label_size = int(data[offset], 2)
            if not label_size: break
            offset += 1 + label_size
            domain += ''.join(map(lambda x: chr(int(x, 2)), data[offset - label_size : offset])) + '.'

        if len(domain): domain = domain[:-1]
        return domain, offset + 1


    # Returns true if port matches with current rule
    def port_matches(self, my_port, rule_port):
        if '-' in rule_port: 
            rule_port = list(map(int, rule_port.split('-')))
            return my_port >= rule_port[0] and my_port <= rule_port[1]
        return rule_port == 'any' or my_port == int(rule_port)


    # Returns true if ip matches with current rule
    def ip_matches(self, my_ip, rule_ip, ip_country):
        if rule_ip in ('any', '0.0.0.0/32, 0.0.0.0/0') or rule_ip in (my_ip, ip_country): return True
        if len(rule_ip) == 2: return False
        if '/' not in rule_ip: rule_ip += '/32'
        if '/' not in my_ip: my_ip += '/32'

        my_mask, rule_mask = map(lambda ip: int(ip[ip.find('/') + 1:]) , [my_ip, rule_ip])
        my_bin, rule_bin = self.ip_to_binary(my_ip, my_mask), self.ip_to_binary(rule_ip, rule_mask)

        if self.debug:
            print('External and rule ips: ', my_ip, rule_ip)
            print('Binary ips: ', my_bin, rule_bin)

        return my_bin.startswith(rule_bin)


    # Sends a packet to the other side
    def send_packet(self, pkt_dir, pkt):
        if self.debug:
            print('-------------------SENDING PACKET-----------------------')
        if pkt_dir == PKT_DIR_INCOMING: self.iface_int.send_ip_packet(pkt)
        elif pkt_dir == PKT_DIR_OUTGOING: self.iface_ext.send_ip_packet(pkt)


    # Returns list of sdn_rules
    def load_rules(self, filename):
        with open(filename) as f:
            return [line.lower().split() for line in f if line.strip() and not line.startswith('%')]


    # Returns dictionary of geo_file, in format range : country
    def load_geo(self, filename):
        with open(filename) as f:
            return [line.split() for line in f]


    # Returns external port (or type) parsed from transport layer
    def parse_transport(self, protocol, data, pkt_dir):
        if protocol == 'icmp':
            return int(data[0], 2)
        elif protocol in ('tcp', 'udp'):
            return (int(data[0] + data[1], 2) if pkt_dir == PKT_DIR_INCOMING else int(data[2] + data[3], 2))
        else :
            return 'unknown protocol'


    # Returns number of protocol inside IP packet, source and dest address and its data
    def parse_IP(self, pkt):
        header_len = int(pkt[0][4:], 2)
        source_ip = self.ip_to_str(pkt[12:16])
        dest_ip = self.ip_to_str(pkt[16:20])
        protocol = PROTOCOLS[int(pkt[9], 2)]
        data = pkt[header_len * 4:]
        return protocol, source_ip, dest_ip, data


    # Takes ip string and converts it to binary data with length of its mask
    def ip_to_binary(self, ip, mask):
        return ''.join(map(lambda x: bin(int(x))[2:].zfill(8), ip[:ip.find('/')].split('.')))[:mask]


    # Converts bytearray to ip string with /32 prefix
    def ip_to_str(self, ip):
        return ''.join(map(lambda s: str(int(s, 2)) + '.', ip))[:-1]