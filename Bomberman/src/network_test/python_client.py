import socket
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
while(True):
    s.sendto(b'JOEEEE BIDENASDASDASDASDASDASDASDASDASD', ('37.100.40.129', 8888))
#s.sendto(b'quit', ('62.98.90.61', 9898))
