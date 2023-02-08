#import socket
#s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
#s.bind(('151.70.135.19', 8888))
#while True:
#    message, sender = s.recvfrom(4096)
#    print(message, sender)
#
#    if message == b'quit':
#        print("QUIT")
#        break
#print(message, sender)

import sys
import socket
import struct
import time
import random
import hashlib

"""
Network messages:

transform/velocity update: 0
player announce (id, transform, name, color): 1
player death: 2
"""


class Player:
    def __init__(self, peer, player_id, name):
        self.peer = peer
        self.transform = (0, 0)
        self.player_id = player_id
        self.name = name
        self.last_packet_timestamp = time.time()

    def get_packet_announce(self):
        return struct.pack("II2f", 1, self.player_id, *self.transform) + self.name

    def get_packet_transform(self):
        return struct.pack("II2f", 0, self.player_id, *self.transform)

    def get_packet_death(self):
        return struct.pack("II", 2, self.player_id)

class Server:
    def __init__(self, address, port, max_packet_size=512):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.bind((address, port))
        self.max_packet_size = max_packet_size
        self.blacklist_client = {}
        self.known_clients = {}
        self.socket.settimeout(1)
        self.players_counter = 1

    def check_status(self):
        now = time.time()
        dead_clients = []
        for peer in self.known_clients:
            player = self.known_clients[peer]
            if now - player.last_packet_timestamp > 5:
                dead_clients.append(peer)

        # announce death
        for dead_peer in dead_clients:
            for peer in self.known_clients:
                current_player = self.known_clients[peer]
                if current_player.peer == dead_peer:
                    continue
                self.socket.sendto(
                    self.known_clients[dead_peer].get_packet_death(),
                    current_player.peer,
                )

        for dead_peer in dead_clients:
            print("killing", dead_peer)
            del self.known_clients[dead_peer]

    def process_message(self, packet, sender):
        # TODO blacklist
        if len(packet) < 4:
            return
        (command_id,) = struct.unpack("I", packet[0:4])
        if command_id == 0:
            player_id, x, y = struct.unpack("Iff", packet[4:16])
            player = self.known_clients[sender]
            if player.player_id != player_id:
               # TODO blacklist
               print('player_id mismatch')
               return
            player.last_packet_timestamp = time.time()
            player.transform = (x, y)
            for peer in self.known_clients:
                current_player = self.known_clients[peer]
                if current_player == player:
                    continue
                # send my position to all of the other players
                self.socket.sendto(player.get_packet_transform(), current_player.peer)

    def run(self):
        while True:
            try:
                packet, sender = self.socket.recvfrom(self.max_packet_size)
            except TimeoutError:
                self.check_status()
                continue
            except:
                continue
            print(packet, sender)
            if sender in self.blacklisted_clients:
                continue
            #if sender in self.challenged_clients:
            #    self.check_challenge(packet, sender)
            #    continue

            self.check_status()
            #self.send_tick()

            if sender in self.known_clients:
                # process message
                self.process_message(packet, sender)
                continue

            if len(self.known_clients) > 23:
                continue

            # new client
            if len(packet) != 4:
                self.blacklist_client(sender)
                continue

            (client_first_challenge,) = struct.unpack("i", packet[0:4])

            if client_first_challenge < 0 or client_first_challenge > 99999:
                self.blacklist_client(sender)
                continue

            self.send_challenge(client_first_challenge, sender)

    #def send_tick(self):
    #    packet = struct.pack("i", 4)
    #    for client in self.known_clients:
    #        current_client = self.known_clients[client]
    #        self.socket.sendto(packet,current_client.peer)

    def send_challenge(self, client_random_value, client_address):
        challenge_value = random.randrange(0, 99999)
        self.challenged_clients[client_address] = (client_random_value, challenge_value)
        packet = struct.pack("i", challenge_value)
        self.socket.sendto(packet, client_address)
        print("ready to challenge", client_address)

    def check_challenge(self, packet, sender):
        client_random_value, server_random_value = self.challenged_clients[sender]
        del self.challenged_clients[sender]
        if len(packet) != 40:
            self.blacklist_client(sender)
            return
        client_plus_server_value = client_random_value + server_random_value
        hash = hashlib.sha256(struct.pack("i", client_plus_server_value)).digest()
        if hash != packet[0:32]:
            self.blacklist_client(sender)
            return
        self.known_clients[sender] = Player(sender, self.players_counter, packet[32:40])
        self.players_counter += 1
        print("Welcome client", sender, self.known_clients[sender].name)
        self.send_world_status(self.known_clients[sender])

    def blacklist_client(self, client_address):
        print("blacklisted", client_address)
        self.blacklisted_clients[client_address] = time.time()

    def send_world_status(self, player):
        for peer in self.known_clients:
            current_player = self.known_clients[peer]
            if current_player == player:
                continue
            # send other player announce to the new user
            self.socket.sendto(current_player.get_packet_announce(), player.peer)
            # send player announce to the other player
            self.socket.sendto(player.get_packet_announce(), current_player.peer)


if __name__ == "__main__":
    server = Server(sys.argv[1], int(sys.argv[2]))
    server.run()