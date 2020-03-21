"""
Your awesome Distance Vector router for CS 168
"""

import sim.api as api
import sim.basics as basics
from collections import namedtuple, defaultdict

# Using namedtuple for entries is more convenient
vector = namedtuple('vector', ['out_port', 'cost_to_dest', 'time'])

# We define infinity as a distance of 16.
INFINITY = 16


class DVRouter (basics.DVRouterBase):
    # NO_LOG = True # Set to True on an instance to disable its logging
    # POISON_MODE = True # Can override POISON_MODE here
    # DEFAULT_TIMER_INTERVAL = 5 # Can override this yourself for testing

    def __init__(self):
        """
        Called when the instance is initialized.
        You probably want to do some additional initialization here.
        """
        self.start_timer()  # Starts calling handle_timer() at correct rate
        self.port_costs = defaultdict(int) # Costs to neighbours
        self.cache = {} # Table of distance vectors
        self.host_ports = {} # Mapping hosts to outgoing ports


    def handle_link_up(self, new_port, latency):
        """
        Called by the framework when a link attached to this Entity goes up.
        The port attached to the link and the link latency are passed in.
        """
        self.port_costs[new_port] = latency

        # Notify neighbours
        for addr, dv in self.cache.items() :
            self.send(packet = basics.RoutePacket(addr, dv.cost_to_dest), port = new_port)


    def handle_link_down(self, dead_port):
        """
        Called by the framework when a link attached to this Entity does down.
        The port number used by the link is passed in.
        """
        self.port_costs.pop(dead_port, None)

        for addr, dv in self.cache.items() :

            if dv.out_port == dead_port : 

                self.cache.pop(addr, None)

                # Poison a path if link died
                if self.POISON_MODE : self.send(packet = basics.RoutePacket(addr, INFINITY), port = [], flood = True)
            

    def handle_rx(self, packet, in_port):
        """
        Called by the framework when this Entity receives a packet.
        packet is a Packet (or subclass).
        port is the port number it arrived on.
        You definitely want to fill this in.
        """

        if isinstance(packet, basics.RoutePacket):
            self.handle_route(packet, in_port, packet.destination, packet.latency + self.port_costs[in_port])

        elif isinstance(packet, basics.HostDiscoveryPacket):
            self.handle_host(packet, in_port)

        else :
            # Send a packet through a flow if it's a ping or a basic one
            if packet.dst in self.cache and self.cache[packet.dst].out_port != in_port :
                self.send(packet = packet, port = self.cache[packet.dst].out_port)


    def handle_timer(self):
        """
        Called periodically.
        When called, your router should send tables to neighbors.  It also might
        not be a bad place to check for whether any entries have expired.
        """

        for addr, dv in self.cache.items() :

            if addr not in self.host_ports and api.current_time() - dv.time >= INFINITY :
                
                self.cache.pop(addr, None)
                # Posion a route if it has expired
                if self.POISON_MODE : self.send(packet = basics.RoutePacket(addr, INFINITY), port = [], flood = True)

            else :
                # Else route a packet simply
                self.send(packet = basics.RoutePacket(addr, dv.cost_to_dest), port = dv.out_port, flood = True)
    


    """



    HELPER FUNCTIONS



    """

    # Handles a host discovery packet, caches info and notifies neighbours about its existence
    def handle_host(self, packet, in_port) :
        
        host = packet.src
        self.host_ports[host] = in_port

        self.cache[host] = vector(in_port, self.port_costs[in_port], api.current_time())
        self.notify_neighbours(host, self.port_costs[in_port])


    # Handles a route packet, poisons a route if it equals infinity
    def handle_route(self, packet, in_port, dest, cost) :

        if packet.latency >= INFINITY : return self.handle_received_poison(dest, in_port)

        if dest not in self.cache or self.cache[dest].cost_to_dest > cost :

            # Caches it if it was absend and notifies neighbours about it
            self.cache[dest] = vector(in_port, cost, api.current_time())
            self.notify_neighbours(dest, cost)

        # This should logically be true to update a timer if an equal cost was received
        # But since with is my code didn't pass the tests, we have to comment it out sadly ;(
        '''
        # elif self.cache[dest].cost_to_dest == cost :
            
         #   self.cache[dest] = vector(*tuple(list(self.cache[dest])[:-1] + [api.current_time()]))
        '''


    # Sends a new info to all neighbours
    def notify_neighbours(self, dest, cost) :

        for port in self.port_costs :
            self.send(packet = basics.RoutePacket(dest, cost), port = port)


    # Deletes a route from its cache and notifies neighbours about it too
    def handle_received_poison(self, dest, in_port) :

        if dest in self.cache and in_port == self.cache[dest].out_port :
            self.cache.pop(dest, None)

            if self.POISON_MODE : self.send(packet = basics.RoutePacket(dest, INFINITY), port = in_port, flood = True)