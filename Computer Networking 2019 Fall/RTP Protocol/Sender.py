import sys
import getopt
from Checksum import validate_checksum, generate_checksum
from collections import defaultdict
import BasicSender

'''
This is a skeleton sender class. Create a fantastic transport protocol here.
'''
class Sender(BasicSender.BasicSender):
    
    def __init__(self, dest, port, filename, debug=False, sackMode=False):
        
        super(Sender, self).__init__(dest, port, filename, debug)
        self.sackMode = sackMode
        self.debug = debug
        self.packet_size = 1450
        self.timeout = 0.5
        self.buffer_size = 7
        self.retransmit_limit = 4


    # Main sending loop.
    def start(self) :

        # Initialize variables, end_seq determines last sequence numbers
        # cum_seq is current cumulative sequence and sacks_send determines number of sacks sent in last iteration
        # Times received stores how many times each seqno was received, cache stores chunks of file
        end_seq, cum_seq, sacks_sent = 0, self.send_handshake(), 0
        times_received, cache = defaultdict(int), self.read_data()
        
        # Send the first data packet
        self.send(self.make_packet('dat', 1, cache[1]))

        while(True) :

            rcv_message = self.receive(self.timeout)

            # If received message is empty, we either break the loop or resend last seqno
            if rcv_message is None or not validate_checksum(rcv_message) : 

                if cum_seq in cache : self.send(self.make_packet('dat', cum_seq, cache[cum_seq]))
                else : break
                continue
            
            # Split a packet
            packet = rcv_message.split('|')
            if packet[0] == 'ack' :
                cum_seq = int(packet[1])
            else :
                # Split it differently if it's a sack
                cum_seq, sacks_sent = self.send_sacks(packet, cache)

            # Break the loop if we reached the last acknowledgement
            if cum_seq not in cache and cum_seq == end_seq : break

            # Fast retransmit packets if necessary
            retransmitted = self.retransmit_packets(times_received, cum_seq, cache)

            # Iterate over each seqno in current window size, limited by retransmissions
            for seqno in range(cum_seq, cum_seq + self.buffer_size - retransmitted - sacks_sent) :

                # Send a packet as usual
                if seqno in cache :
                    self.send(self.make_packet('dat', seqno, cache[seqno]))

                # Send a fin packet if we reached the end
                elif seqno not in cache and end_seq == 0 :
                    self.send(self.make_packet('fin', seqno, ''))
                    end_seq = seqno + 1
                    #
                    break


    # Parses a sack message and sends every every requested chunk
    def send_sacks(self, packet, cache) :

        comma = packet[1].index(';')
        cum_seq = int(packet[1][0 : comma])
        
        sack_ls = ([] if packet[1].endswith(';') else list(map(lambda x : int(x.replace(';', '')), packet[1][comma + 1:].split(','))))
        for seqno in sack_ls :
            if seqno in cache : self.send(self.make_packet('dat', seqno, cache[seqno]))

        return cum_seq, len(sack_ls)


    # Fast retransmission :
    # If we get same ack 4 times in a row, we retransmit that packet only
    def retransmit_packets(self, times_received, seqno, cache) :

        times_received[seqno] += 1
        if times_received[seqno] >= self.retransmit_limit and seqno in cache: 
            self.send(self.make_packet('dat', seqno, cache[seqno]))
            times_received[seqno] = 0
            return 1

        return 0


    # Splits data into chunks, creates a dict mapping sequence numbers to file chunks
    def read_data(self) :

        data, cur_ind, cache = self.infile.read(), 1, {}
        for i in range(0, len(data), self.packet_size) :
            cache[cur_ind] = data[i : i + self.packet_size]
            cur_ind += 1
        return cache


    # Sends a syn packet to received, initiating contact
    # Will run until receiver returns an ack with seqno 1
    def send_handshake(self) :

        self.send(self.make_packet('syn', 0, ''))
        rcv_message = self.receive(self.timeout)
        return 1 if rcv_message is not None and validate_checksum(rcv_message) else self.send_handshake()
    

    # Logs a packet for debugging purposes 
    def print_packet(self, packet) :
        if self.debug or True : print(packet)

'''
This will be run if you run this script from the command line. You should not
change any of this; the grader may rely on the behavior here to test your
submission.
'''
if __name__ == "__main__":
    def usage() :
        print ("BEARS-TP Sender")
        print ("-f FILE | --file=FILE The file to transfer; if empty reads from STDIN")
        print ("-p PORT | --port=PORT The destination port, defaults to 33122")
        print ("-a ADDRESS | --address=ADDRESS The receiver address or hostname, defaults to localhost")
        print ("-d | --debug Print debug messages")
        print ("-h | --help Print this usage message")
        print ("-k | --sack Enable selective acknowledgement mode")

    try:
        opts, args = getopt.getopt(sys.argv[1:],
                               "f:p:a:dk", ["file=", "port=", "address=", "debug=", "sack="])
    except:
        usage()
        exit()

    port = 33122
    dest = "localhost"
    filename = None
    debug = False
    sackMode = False

    for o,a in opts:
        if o in ("-f", "--file="):
            filename = a
        elif o in ("-p", "--port="):
            port = int(a)
        elif o in ("-a", "--address="):
            dest = a
        elif o in ("-d", "--debug="):
            debug = True
        elif o in ("-k", "--sack="):
            sackMode = True

    s = Sender(dest,port,filename,debug, sackMode)
    try:
        s.start()
    except (KeyboardInterrupt, SystemExit):
        exit()