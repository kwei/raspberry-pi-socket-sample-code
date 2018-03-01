from network import LoRa
import socket

#host=""
#port="5560"

# Initialize LoRa in LORA mode more params can be given, like frequency, tx power, spreading factor, etc
lora = LoRa(mode=LoRa.LORA) 

# create a raw LoRa socket
s = socket.socket(socket.AF_LORA, socket.SOCK_RAW)
#s.bind((host, port))
s.setblocking(False)

# send some data
s.send('Hello from the LoPy')

# get any data received...
data = s.recv(64)
print(data)
