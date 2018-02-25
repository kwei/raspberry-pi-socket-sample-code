import socket
import sys

host = ''
port = 5560

storedValue = "[ Info ] This is testing messages."

def setupServer():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print("Socket created.")
    try:
        s.bind((host, port))
    except socket.error as msg:
        print(msg)
    #print("Socket bind complete.")
    return s

def setupConnection():
    s.listen(1) # Allow one connection at a time.
    #print("[ Listening ]")
    conn, address = s.accept()
    print("Connected to : " + address[0] + ":" + str(address[1]))
    return conn

def GET():
    reply = storedValue
    return reply

def REPEAT(dataMessage):
    reply = dataMessage[1]
    return reply

def dataTransfer(conn):
    # A big loop that sends/receives data until told not to.
    while True:
        #print("Set blocking to recv data")
        data = conn.recv(1024) # receive the data.
        #print("received!")
        data = data.decode('utf-8')
        #print("# Split the data such that you seperate the command from the rest of the data")
        dataMessage = data.split(' ', 1)
        command = dataMessage[0]
        #print("==== if else if ====")
        if command == 'GET':
            reply = GET()
        elif command == 'REPEAT':
            reply = REPEAT(dataMessage)
        elif command == 'EXIT':
            print("Client leave.")
            break
        elif command == 'KILL':
            print("Server is shutting down.")
            sys.exit()
            break
        else:
            reply = 'Unknown Command'
        # Send the reply back to the client
        conn.sendall(str.encode(reply))
        print("Data has been sent!")
    conn.close()

# Main

s = setupServer()
#print("Waiting...")
while True:
    try:
        #print("Try to setup connect")
        conn = setupConnection()
        #print("setupConnected and ready to dataTransfer")
        dataTransfer(conn)
    except:
        print("Bye Bye.")
        break
