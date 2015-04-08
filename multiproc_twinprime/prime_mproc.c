/*
 *	Travis J. Sanders
 *	sandetra@onid.oregonstate.edu
 *	CS344-400
 *	Homework #5
 *
 *	Multi-process twin-prime number generator
 *
 *	Link with -lm and -lrt
 */
#define _XOPEN_SOURCE 700
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <getopt.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>		//reaping
#include <semaphore.h>
#include <signal.h>
#include <errno.h>

/*	Bit array access macros
 *	http://www.tek-tips.com/faqs.cfm?fid=3999
 *	http://stackoverflow.com/questions/22006507/thread-safe-bit-array
 */
#define ISBITSET(x,i) ((x[i>>3] & (1<<(i&7)))!=0)
#define SETBIT(x,i) __sync_or_and_fetch(&x[i>>3], 1U << (i&7));
#define CLEARBIT(x,i) x[i>>3]&=(1<<(i&7))^0xFF;

int find_twins(int* list, unsigned int n, int verbose);
int sieve_primes(int * list, unsigned int n);
void * mount_shmem(const char * path, size_t size);

sem_t *sem;
sem_t *sem1;
int *topstack;

/*	Multi-process prime number finder function.
 *	Pass data struct with size, bit array pointer 
 */
int sieve_primes(int* list, unsigned int n)
{
	unsigned int k;
	unsigned int m, i;

	k = 1;

	while (k < sqrt(n)) {
		m = k+1;
		i = 2;

		/*	Critical portion	*/
		sem_wait(sem);
		if (m < topstack[0]) m = topstack[0] + 1;
		while ((m < n) && ISBITSET(list, m)) m++;
		if (m > *topstack) *topstack = m;
		SETBIT(list, m);
		sem_post(sem);

		/* Set all multiples as composite */
		while ((long)m*i < n) {
			if (!ISBITSET(list, m*i)) SETBIT(list, m*i);
			i++;
		}
		/*	Second critical portion. Clear prime bit.	*/
		sem_wait(sem1);
		CLEARBIT(list, m);
		sem_post(sem1);
		k = m;
	}
	return 0;
}

/*	Finds all twin primes in bit array (list) of size (n) 
 *	-q flag included
 */
int find_twins(int* list, unsigned int n, int quiet)
{
	unsigned int j, t;
	t = 0;
	for (j=3; (j+2 < n); j+=2) {
		if (!ISBITSET(list, j)) {
			if (!ISBITSET(list, (j+2))) {
				if (quiet == 1) printf("%u %u\n", j, j+2);
				t++;
			}
		}
	}
	return t;
}

//TLPI 54-1, lecture
void *mount_shmem(const char *path, size_t size)
{
	int fd;
	void *addr;

	fd = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror("SHM OPEN FAIL");
		_exit(EXIT_FAILURE);
	}
	if (ftruncate(fd, size) == -1) {
		perror("TRUNCATE FAIL");
		_exit(EXIT_FAILURE);
	}
	/* Map shared memory object */
	addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
	if (addr == MAP_FAILED) {
		perror("MAP FAIL");
		_exit(EXIT_FAILURE);
	}
	return addr;
}

static void sigHandler(int sig)
{
	int sav;
	sav = errno;
	sem_close(sem);
	sem_close(sem1);
	sem_unlink("/sandetra_semaphore");
	sem_unlink("/sandetra_semaphore1");
	shm_unlink("/sandetra_mproc");
	while (waitpid(-1, NULL, WNOHANG) > 0) continue;
	errno = sav;
	_exit(EXIT_FAILURE);
}

static void sigHandler1(int sig)
{
	int sav;
	sav = errno;
	while (waitpid(-1, NULL, WNOHANG) > 0) continue;
	errno = sav;
}
 
int main(int argc, char *argv[]) 
{
	unsigned int i, c;
	int quiet, verbose, procs;
	unsigned int m;

	int * list;
	void * addr;
	size_t size;

    const char * file_name;

    //Lecture19
    sem = sem_open("/sandetra_semaphore", O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem1 = sem_open("/sandetra_semaphore1", O_CREAT, S_IRUSR | S_IWUSR, 1);

    if (!sem | !sem1) {
    	perror("SEMBROKE");
    	exit(EXIT_FAILURE);
    }

	file_name = "/sandetra_mproc";

	if ((signal(SIGINT, sigHandler) == SIG_ERR) |
		(signal(SIGCHLD, sigHandler1) == SIG_ERR)) {
		perror("SIG handlers");
		_exit(EXIT_FAILURE);
	}

	/* DEFAULT VALUES */
	m = UINT_MAX;
	quiet = 1;
	procs = 1;
	verbose = 0;

	/* Parse arguments */
	while ((c = getopt (argc, argv, "vqm:c:")) != -1) {
		switch(c) {
		case 'v':
			verbose = 1;
			break;
		case 'q':
			quiet = 0;
			break;
		case 'm':
			m = atoi(optarg);
			if (m < 10) m = 10; //for safety
			break;
		case 'c':
			procs = atoi(optarg);
			if (c < 1) c = 1; //for safety
			break;
		default:
			break;
		}
	}

	size = m;
	size += sizeof(int) + 1;
	addr = mount_shmem(file_name, size);
	list = (int*)addr;
	topstack = (int*)(list + m/sizeof(int));

	topstack[0] = 2;

	if (verbose == 1) {
		printf("Finding twin primes under m=%u\n", m);
		printf("The square root of m is =%f\n", sqrt(m));
	}

	/* Fork processes */
	for (i=0; i<procs; i++) {
		switch(fork()) {
		case -1:
			perror("FORK!");
			_exit(EXIT_FAILURE);
		case 0:
			sieve_primes(list, m);
			exit(EXIT_SUCCESS);
		default:
			break;
		}
	}

	/* Wait on children */
	for (i=0; i<procs; i++) {
		wait(NULL);
	}

	/* Find twins */
	i = find_twins(list, m, quiet);

	if (verbose == 1) 
		printf("Twin primes under [%u] : %d\n",m,i);

	sem_close(sem);
	sem_close(sem1);
	if ((sem_unlink("/sandetra_semaphore") == -1) | 
		(sem_unlink("/sandetra_semaphore1") == -1)) {
		perror("Semaphore closing");
		exit(EXIT_FAILURE);
	}
	if (shm_unlink(file_name) == -1) {
		perror("shm_unlink");
		_exit(EXIT_FAILURE);
	}
	return 0;
}
