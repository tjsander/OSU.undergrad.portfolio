/*
# Travis J. Sanders
# sandetra@onid.oregonstate.edu
# CS344-400
# Homework #3

Simple signal handlers.

Refered to: Piazza @98 @99
*/
#define _POSIX_SOURCE
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void sigHandler1(int sig)
{
	printf("SIGUSR1 has been caught \n");
}

static void sigHandler2(int sig)
{
	printf("SIGUSR2 has been caught\n");
}

static void sigHandler3(int sig)
{
	printf("SIGINT has been caught, terminating the program\n");
	_exit(EXIT_FAILURE);
}

int main (int argc, char *argv[])
{
	if (signal(SIGUSR1, sigHandler1) == SIG_ERR) 
		printf("Something terrible has happened to SIGUSR1\n");
	if (signal(SIGUSR2, sigHandler2) == SIG_ERR)
		printf("Something terrible has happened to SIGUSR2\n");
	if (signal(SIGINT, sigHandler3) == SIG_ERR)
		printf("Something terrible has happened to SIGINT\n");

	kill(getpid(), SIGUSR1);
	sleep(3);
	kill(getpid(), SIGUSR2);
	sleep(3);
	kill(getpid(), SIGINT);

	printf("I should never see this.\n");
	return 0;
}