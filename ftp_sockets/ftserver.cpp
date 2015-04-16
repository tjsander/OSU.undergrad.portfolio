/*	
 *	Travis J. Sanders
 *	sandetra@onid.oregonestate.edu
 *  CS 372 Introduction to Computer Networks 
 *  Programming Assignment #2
 *  Feb 25, 2015
 *
 *	Adapted from http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 *	SPECIFICALLY: server.c -- a stream socket server demo
 *  ALSO MODIFIED FROM PROGRAMMING ASSIGNMENT 1
 */

#include <iostream>
#include <dirent.h>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>

#define BACKLOG 10		// how many pending connections queue will hold
#define MAX_CHARS 1024	// maximum message / buffer size
#define LOUD 0

#define SERVER_NAME "sandetra-ftserver"

int send_directory_contents(int new_fd, const char* path);
int send_file(int new_fd, const char* filename, char s[INET6_ADDRSTRLEN], int port);
int send_message(int new_fd, const char* input);

// Signal handler for child processes.
void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
    	return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*  Checks the fd for an incoming message. Returns -1 if the client
 *      has disconnected or sends an empty message. Prints message to
 *      stdout and returns 0 on success.
 */
int get_message(int new_fd)
{
	char buf[MAX_CHARS];
	int got;

	if ((got = recv(new_fd, buf, MAX_CHARS, 0)) == -1) {
		perror("recv");
		return(-1);
	}
	buf[got] = '\0';

	// Close connection if an empty message is received.
	if (strcmp(buf, "") == 0) return -1;
	printf("%s\n", buf);

	// if (LOUD) printf("Received chars=%i\n", got);
	return 0;
}

/*  Sets up a data connection with address s on port
 *	Either transmits filename or a list (if filename is NULL)
 */
int data_connection(char s[INET6_ADDRSTRLEN], int port, const char* filename)
{
	int sockfd, stop;
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_pton(AF_INET, s, &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	if (filename == NULL) {
		printf("Sending directory contents to %s:%i\n", s, port);
		send_directory_contents(sockfd, NULL);
	} else {

		if (send_file(sockfd, filename, s, port) == -1) {
			close(sockfd);
			// printf("File not sent.\n");
			return -1;
		}
	}

	close(sockfd);
	return 0;
}

/*  Listens on control connection new_fd for commands
 *  - Commands must begin with "PORT "
 *	- Followed by either "LIST" or "FILE " + filename
 */
int get_command(int new_fd, char s[INET6_ADDRSTRLEN])
{
	char buf[MAX_CHARS];
	int got, port;
	char* filename;

	if ((got = recv(new_fd, buf, MAX_CHARS, 0)) == -1) {
		perror("recv");
		return(-1);
	}

	buf[got] = '\0';
	if (LOUD) printf("Received: %s\n", buf);

	// Close connection if an empty message is received.
	if (strcmp(buf, "") == 0 || strlen(buf) < 10) return -1;
	// printf("%s\n", buf);

	if (strncmp(buf, "PORT", 4) == 0) {
		buf[10] = '\0';
		port = atoi(&buf[5]);
		if (LOUD) printf("PORT=%i\n",port);
	} else printf("ERROR: Unrecognized command 1 '%s'\n", buf);

	if (strncmp(&buf[11], "LIST", 4) == 0) {
		printf("List directory requested on port %i\n", port);
		data_connection(s, port, NULL);
		if (strlen(buf) > 16) { printf("Dir=%s\n",&buf[15]); }
	} else if (strncmp(&buf[11], "FILE", 4) == 0) {
		filename = &buf[16];
		printf("File '%s' requested on port %i\n", filename, port);
		if (data_connection(s, port, filename) == -1) {
			send_message(new_fd, "ERROR File not found.");
			printf("ERROR: FILE NOT FOUND\n");
		}
	} else printf("ERROR: Unrecognized command 2 '%s'\n", buf);
	
	return 0;
}


/* Sends a message into new_fd
 * returns 0 on success, -1 on failure
 */
int send_message(int new_fd, const char* input)
{
	char send_me[MAX_CHARS];

	bzero(send_me, MAX_CHARS);
	strcpy(send_me, input);
	// if (strcmp(send_me,"") == 0) return -1;

	if (send(new_fd, send_me, strlen(send_me), 0) == -1) {
		perror("send");
		return -1;
	}
	return 0;
}

/*	Sends a list of directory contents on new_fd
 *	Default path is same directory as server unless specified
 */
int send_directory_contents(int new_fd, const char* path)
{
	// Directory iterator modified from:
	// http://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
	DIR *dir;
	struct dirent *ent;
	char send_me[MAX_CHARS];
	if (path == NULL) path = "./";

	bzero(send_me, MAX_CHARS);

	strcpy(send_me, "");

	if ((dir = opendir (path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			// printf ("%s\n", ent->d_name);
			strcat(send_me, ent->d_name);
			strcat(send_me, "\n");
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		return EXIT_FAILURE;
	}
	send_message(new_fd, send_me);
	return 0;
}

/* Sends a message into new_fd
 * returns 0 on success, -1 on failure
 */
int send_file(int new_fd, const char* filename, char s[INET6_ADDRSTRLEN], int port)
{
	// http://www.cprogramming.com/tutorial/cfileio.html
	FILE *fp;
	char c[MAX_CHARS];
	int i, fd;
	int total;
	struct stat buf;

	fp=fopen(filename, "r");

	// Return if file does not exist
	if (fp == NULL) {
		return -1;
	}

	fd = fileno(fp);
	fstat(fd, &buf);
	int size = buf.st_size;

	printf("Sending '%s' to %s:%i\n", filename, s, port);
	if (LOUD) printf("Size of requested file is %i\n", size);

	i = 0;
	total = 0;
	c[i] = fgetc(fp);

	// while(c[i] != EOF) {
	while(total < size) {
		i++;
		total++;
		if (i == MAX_CHARS) {
			if (write(new_fd, c, sizeof(char) * i) == -1) {
				perror("send");
				return -1;
			}
			bzero(c, MAX_CHARS);
			i = 0;
		}
		c[i] = fgetc(fp);
	}
	if (write(new_fd, c, sizeof(char) * i) == -1) {
		perror("send");
		return -1;
	}
	fclose(fp);

	close(new_fd);
	if (LOUD) printf("File transfer completed!\n");
	if (LOUD) printf("Transmitted bytes=%i\n", total);
	return 0;
}

/* Starts a new server on 'sockfd'
 */
void start_server(int sockfd)
{
	int new_fd;
	int loop_stop = 0;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	char s[INET6_ADDRSTRLEN];
	char send_me[MAX_CHARS];

	while(loop_stop == 0) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
		printf("Connection from %s\n", s);

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener

			get_command(new_fd, s);

			close(new_fd);

			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}
}

// Sets up a new socket on port
int socket_setup(char* port)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	struct sigaction sa;
	int yes=1;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
			p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
	return sockfd;
}

/*  Adapted from http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
 *  SPECIFICALLY: server.c -- a stream socket server demo
 */
int main(int argc, char *argv[])
{
	int sockfd;
	char* port;

	if (argc > 1) {
		// printf("Port is %s\n", argv[1]);
		port = argv[1];
	} else {
		printf("usage: ./chatserve PORTNUMBER\n");
		return -1;
	}

	sockfd = socket_setup(port);
	// Start the server
	printf("Server open on %s\n", port);

	// send_file(sockfd, "README.txt");
	start_server(sockfd);

	return 0;
}
