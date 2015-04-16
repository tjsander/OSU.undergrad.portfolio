Travis J. Sanders
CS 372 Introduction to Computer Networks 
Programming Assignment #2
Feb 25, 2015
------------------------------------------------------------------------------
FTSERVER
--------
MAKEFILE:
make
	make ftserver
make clean
	cleans .o and application files
make cleanall
	removes files with 'copy_' prefix as well as .o and application files

./ftserver PORT
	Runs ftserver. Listens on PORT

------------------------------------------------------------------------------
FTCLIENT
--------

python chatclient.py HOSTNAME PORT -l REQUEST_PORT
	connect with HOSTNAME on port PORT and request a list on REQUEST_PORT

python chatclient.py HOSTNAME PORT -g FILENAME REQUEST_PORT
	connect with HOSTNAME on port PORT and request file FILENAME on 
	REQUEST_PORT

Timeout is 3 seconds. Should fail gracefully.
If file exists in client directory, prepends 'copy_' to filename
Works with all filetypes. Tested with text, images, mp3, pdf...

------------------------------------------------------------------------------
EXTRA CREDIT:
-------------

1. Transfer files additional to text files (e.g. binary files) (a text file with 
a non-.txt extension doesn’t count.

My program works with binary files. Tested with MP3s, images, videos.

2. Make your server multi-threaded.

Both my server and client are multi-process. 
	Server (C++) uses fork()
	client (Python) uses Process()

