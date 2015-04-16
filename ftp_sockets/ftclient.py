# Travis J. Sanders
# CS 372 Introduction to Computer Networks 
# Programming Assignment #2
# Feb 25, 2015

# MODIFIED FROM https://docs.python.org/2/library/socket.html
# SPECIFICALLY: Echo client program (IPv4)
# ALSO MODIFIED FROM PROGRAMMING ASSIGNMENT 1

import socket
import sys
import errno
import signal
import time
# http://stackoverflow.com/questions/82831/check-if-a-file-exists-using-python
import os.path
# https://docs.python.org/3/library/multiprocessing.html
from multiprocessing import Process

MAX_CHARS = 1024

# Gets user input and sends a message through socket s from "username"
# Returns 0 on success.
# Returns 1 if the server has terminated.
def send_message(s, username):
    # https://docs.python.org/2/library/functions.html#raw_input
    string_in = raw_input(username + ' >> ')
    string_out = username + " >> " + string_in

    if string_in == '\quit':
        return 1;
    if string_in == "":
        send_message(s, username)
        return 0;
    try:
        s.sendall(string_out)
    except:
        # print "Server has disconnected."
        return -1;

    return 0;


# Receives a message from socket s and prints to the console.
# Returns 0 on success.
# Returns 1 if the server has terminated.
def get_message(s):
    while 1:
        data = s.recv(MAX_CHARS)
        if not data:
            return -1;
        print data
    return 0;


# Starts a client on HOST: PORT
# Sends a request for data connection on REQ_PORT
# Optionally: requests a file FILENAME, otherwise requests a list
# Loops until user enters '\quit' command
def start_client(HOST, PORT, REQ_PORT, FILENAME):
    if FILENAME == 0:
        p = Process(target=get_dir, args=(REQ_PORT,))
    else:
        p = Process(target=get_file, args=(REQ_PORT, FILENAME,))
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect((HOST, PORT))
# http://stackoverflow.com/questions/5161167/python-handling-specific-error-codes
    except socket.error, v:
        errorcode=v[0]
        if errorcode==errno.ECONNREFUSED:
            print "Connection refused by", HOST, PORT
        s.close()
        # p.join()
        sys.exit()
    p.start()
    time.sleep(.3) # Necessary to prevent server from connecting before socket is active
    
    if FILENAME == 0:
        # s.sendall("PORT " + str(REQ_PORT));  //UNCOMMENT TO TEST SERVER TIMEOUT
        s.sendall("PORT " + str(REQ_PORT) + " LIST");
    else:
        s.sendall("PORT " + str(REQ_PORT) + " FILE " + FILENAME);
    while 1:
        data = s.recv(MAX_CHARS)
        if not data: break
        print HOST, PORT, 'says', data
        p.terminate()
        return -1
    s.close()
    p.join()


# Sets up a data connection on PORT to listen for incoming messages.
# https://docs.python.org/2/library/socket.html
def get_dir(PORT):
    HOST = ''                 # Symbolic name meaning all available interfaces
    serv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serv.bind((HOST, PORT))
    serv.settimeout(3)
    try:
        serv.listen(1)
        conn, addr = serv.accept()
    except socket.timeout, v:
        print "ERROR: Server timed out"
        sys.exit()
    print 'Receiving directory structure from', addr[0], PORT
    while 1:
        data = conn.recv(MAX_CHARS)
        if not data: break
        print data
    conn.close()


# Sets up a connection on PORT and receives a file of FILENAME
# Times out after 3 seconds
# https://docs.python.org/2/library/socket.html
def get_file(PORT, FILENAME):
    HOST = ''                 # Symbolic name meaning all available interfaces
    filestring = FILENAME
    i = 0
    serv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serv.bind((HOST, PORT))
    serv.settimeout(3)
    
    try:
        serv.listen(1)
        conn, addr = serv.accept()
    except socket.timeout, v:
        print "ERROR: Server timed out"
        sys.exit()

    print 'Data connection to', addr[0], PORT, "successful"
    printme = ""
    while 1:
        data = conn.recv(MAX_CHARS)
        if not data: break
        printme += data

    while os.path.isfile(filestring):
        filestring = "copy_" + filestring
    f = open(filestring, 'w')
    f.write(printme)
    conn.close()
    # print printme
    print 'File transfer complete.'


#signal handler for sigint
#http://stackoverflow.com/questions/1112343/how-do-i-capture-sigint-in-python
def signal_handler(signal, frame):
    sys.exit(0)


def main():
    signal.signal(signal.SIGINT, signal_handler)
    if (len(sys.argv) >=5):
        HOST = sys.argv[1]
        PORT = sys.argv[2]
        FLAG = sys.argv[3]
        REQ_PORT = sys.argv[4]
        if (len(sys.argv) > 5):
            FILENAME = sys.argv[4]
            REQ_PORT = sys.argv[5]
        
        # print "Travis Sanders <sandetra> Network Project 2"
        # print "Connecting to " + HOST + ":" + PORT
        if int(REQ_PORT) >= 65535:
            print "Port too high. Please chose a port under 65535"
            sys.exit(-1)
        if int(REQ_PORT) <= 1024:
            print "Port too low. Please chose a port above 1024"
            sys.exit(-1)
        if FLAG == "-l":
            start_client(HOST, int(PORT), int(REQ_PORT), 0)
        elif FLAG == "-g":
            print "Getting file...", FILENAME
            start_client(HOST, int(PORT), int(REQ_PORT), FILENAME)
        else:
            print "Unrecognized flag", FLAG
            print "Use -l or -g"
    else:
        print "Insufficient parameters. use: python chatclient.py HOSTNAME PORT -l REQUEST_PORT"
        print "or: python chatclient.py HOSTNAME PORT -g FILENAME REQUEST_PORT"


if __name__ == "__main__":
    main()

